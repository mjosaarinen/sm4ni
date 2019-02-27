# Makefile
# 2018-04-20  Markku-Juhani O. Saarinen <mjos@iki.fi>

BIN		= xtest
OBJS		= sm4ni.o sm4_ref.o testmain.o
DIST		= sm4ni

CC		= gcc
CFLAGS		= -Wall -Ofast -march=native -DSM4NI_UNROLL

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) -o $(BIN) $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(BIN)
