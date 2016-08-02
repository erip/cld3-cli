//g++ cld3_cli_test.cc -o test -std=c++14 -lstdc++fs -I${CATCH_DIR}
OBJS = cld3_cli_test.o
CC = g++
DEBUG = -g
INC = -I${CATCH_DIR} -I${THIRD_PARTY} -I${JSONCONS} -I${PROTOBUF}
CXXFLAGS = -Wall -Werror -std=c++14 -lstdc++fs -c $(DEBUG) $(INC) -L${THIRD_PARTY} -L${PROTOBUF}
LFLAGS = -Wall -Werror -std=c++14 -lstdc++fs  $(DEBUG) $(INC) -L${THIRD_PARTY} -L${PROTOBUF_LIBS}
LIBS = -lcld3_full -lprotobuf

test_cld3_cli : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o test_cld3_cli

cld3_cli_test.o : cld3_cli_test.cc
	$(CC) $(CXXFLAGS) cld3_cli_test.cc

*.o: -I${THIRD_PARTY}/*.cc
	$(CC) $(CXXFLAGS) *.cc

clean:
	rm -rf *.o *~ test_cld3_cli test_cld3_cli.dSYM
