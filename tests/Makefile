//g++ cld3_cli_test.cc -o test -std=c++14 -lstdc++fs -I${CATCH_DIR}
OBJS = cld3_cli_test.o
INC = -I${CATCH_DIR} -I${THIRD_PARTY}
CC = g++
DEBUG = -g
CFLAGS = -Wall -Werror -std=c++14 -lstdc++fs -c $(DEBUG) $(INC)
LFLAGS = -Wall -Werror -std=c++14 -lstdc++fs  $(DEBUG) $(INC)

test_cld3_cli : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o test_cld3_cli

cld3_cli_test.o : cld3_cli_test.cc
	$(CC) $(CFLAGS) cld3_cli_test.cc

clean:
	rm -rf *.o *~ test_cld3_cli test_cld3_cli.dSYM
