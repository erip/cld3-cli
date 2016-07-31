OBJS = cld3_cli.o
CC = g++
DEBUG = -g
INC = -I${THIRD_PARTY}
CFLAGS = -Wall -Werror -std=c++14 -lstdc++fs -c $(DEBUG) $(INC)
LFLAGS = -Wall -Werror -std=c++14 -lstdc++fs  $(DEBUG) $(INC)

cld3_cli : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o cld3_cli

cld3_cli.o : cld3_cli.h cld3_cli.cc
	$(CC) $(CFLAGS) cld3_cli.cc

clean:
	rm -rf *.o *~ cld3_cli cld3_cli.tar cld3_cli.dSYM && make clean -C ./tests

tar:
	make clean && tar cfv cld3_cli.tar Makefile cld3_cli.h cld3_cli.cc ./tests/Makefile ./tests/cld3_cli_test.cc

test:
	make -C ./tests && ./tests/test_cld3_cli && make clean -C ./tests
