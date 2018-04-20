# sm4ni

Demonstration that AES-NI instructions and affine transforms can be used to create a fast, vectorized,
constant tyime implmementation of the Chinese Encryption Standard SM4

## Background and Theory

SM4 is the Chinese Standard Encryption Algorithm. It is a block cipher with
a 128-bit key and 128-bit block size. For more information, see
the [Internet Draft](https://www.ietf.org/id/draft-ribose-cfrg-sm4).
The use of SM4 is now mandated for certain applications within China.
ARM is introducing special SM4 instructions in its future architectures.

This note shows how to use Intel vector instructions to create about 260%
faster **constant time** implementation. The trick is to use affine 
transforms to emulate the SM4 S-Box with the AES S-Box. The S-Boxes are
both based on inversion, but use different affine transforms and even
polynomial basis for the finite field. However, different polynomial
bases are affine isomorphic, so two affine transforms A1 and A2 suffice.

```
SM4-S(x) = A2(AES-S(A1(x))
A1(x) = M1*x + C1
A2(X) = M2*x + C2
```
Here affine transform consists of a multiplication with a 8x8 binary
matrix and addition of a constant. We note that affine transforms can 
be implemented with a couple of byte shuffle instructions. We use the 
`AESENCLAST` instruction (nominally intended for last round) 
in order to avoid MDS matrix computations when doing the AES S-Box
computations.

Due to the structure of SM4, we are processing 4 blocks in parallel.
This means that CBC cannot be implemented this way, but CTR and GCM
are fine. This code example only implements the encryption function 
and use Intel C intrinsics. The fast block encryption code is in
`sm4ni.c`.

## Testing

Just clone or extract he distibution and:
```
$ make
gcc -Wall -Ofast -march=native  -c sm4ni.c -o sm4ni.o
gcc -Wall -Ofast -march=native  -c sm4_ref.c -o sm4_ref.o
gcc -Wall -Ofast -march=native  -c testmain.c -o testmain.o
gcc  -o xtest sm4ni.o sm4_ref.o testmain.o 

$ ./xtest 
SM4 reference     60.906 MB/s
Vector SM4NI     160.666 MB/s
mjos@blu:~/Desktop/prj/sm4ni$ 
```
Of course support for AES-NI is required. The benchmark indicates 264%
speed for the new implementation (and it is constant time). Your
architecture may give different results. Futher optimizations are
possible.

## Notes

This is part of ongoing research work, and I think I am the first person who
discovered this trick. So please give me some credit if you use this.

