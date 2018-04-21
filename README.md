# sm4ni

Demonstration that AES-NI instructions and affine transforms can be used 
to create a fast, vectorized,constant time implementation of the Chinese 
Encryption Standard SM4.

## Background and Theory

SM4 is the Chinese Standard Encryption Algorithm. It is a block cipher 
with a 128-bit key and 128-bit block size. For more information, see
the [Internet Draft](https://www.ietf.org/id/draft-ribose-cfrg-sm4).
The use of SM4 is now mandated for certain applications within China.
ARM is introducing special SM4 instructions in its future architectures.

This note shows how to use Intel vector instructions to create about 2-3
times faster **constant time** implementation. The trick is to use affine 
transforms to emulate the SM4 S-Box with the AES S-Box. The S-Boxes are
both based on finite field inversion, but use different affine transforms 
and even polynomial basis for the finite field. However, different 
polynomial bases are affine isomorphic. 

We combine various linear operations into two affine transforms (one on 
each side), A1 and A2. Here affine transform consists of a multiplication 
with a 8x8 binary matrix M and addition of a 8-bit constant C.
```
SM4-S(x) = A2(AES-S(A1(x))
A1(x) = M1*x + C1
A2(x) = M2*x + C2
```
We note that each affine transform can be constructed from XOR of two 
4x8-bit table lookups, which we implement with constant time byte 
shuffle instructions (each 16-entry table is in a single 128-bit register).
For parallel AES S-Box lookups we use the `AESENCLAST` instruction 
(nominally intended for AES last round) in order to avoid AES MDS matrix 
expansion.

Due to the structure of SM4, we are processing 4 blocks in parallel.
This means that CBC cannot be implemented this way, but faster parallelizable
modes like CTR, GCM, and OCB are fine. This code example only implements 
the block encryption function (block decryption is essentially equivalent but unneeded for decryption with CTR, GCM, OCB) and uses Intel C intrinsics. The 
fast block encryption code is in `sm4ni.c`.

## Testing

Just clone or extract the distibution and:
```
$ make
gcc -Wall -Ofast -march=native  -c sm4ni.c -o sm4ni.o
gcc -Wall -Ofast -march=native  -c sm4_ref.c -o sm4_ref.o
gcc -Wall -Ofast -march=native  -c testmain.c -o testmain.o
gcc  -o xtest sm4ni.o sm4_ref.o testmain.o 

$ ./xtest 
SM4 reference     60.906 MB/s
Vector SM4NI     160.666 MB/s
```
Of course support for AES-NI is required. This benchmark indicates 264%
speed for the new implementation (and it is constant time!). Your
architecture may give very different results. Futher optimizations are
possible.

## Notes

This is part of ongoing research work, and I think I am the first person who
discovered this trick. So please give me some credit if you use this.

