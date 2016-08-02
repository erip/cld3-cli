OBJS = cld3_cli.o
CC = g++
DEBUG = -g
INC = -I${THIRD_PARTY} -I${JSONCONS} -I${PROTOBUF}
CXXFLAGS = -Wall -Werror -std=c++14 -lstdc++fs -c $(DEBUG) $(INC) -L${THIRD_PARTY} -L${PROTOBUF}
LFLAGS = -Wall -Werror -std=c++14 -lstdc++fs  $(DEBUG) $(INC) -L${THIRD_PARTY} -L${PROTOBUF_LIBS}
LIBS = -lcld3_full -lprotobuf

cld3_cli : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) -o cld3_cli

cld3_cli.o : cld3_cli.h cld3_cli.cc
	$(CC) $(CXXFLAGS) $(LIBS) cld3_cli.cc

clean:
	rm -rf *.o *~ cld3_cli cld3_cli.tar cld3_cli.dSYM && make clean -C ./tests

tar:
	make clean && tar cfv cld3_cli.tar Makefile cld3_cli.h cld3_cli.cc ./tests/Makefile ./tests/cld3_cli_test.cc

test:
	make -C ./tests && ./tests/test_cld3_cli && make clean -C ./tests
