OBJS = cld3_cli_test.o
CC = g++
DEBUG = -g
INC = -I${CATCH_DIR} -I${CLD3} -I${JSONCONS} -I${PROTOBUF}
CXXFLAGS = -Wall -Werror -std=c++14 -lstdc++fs -c $(DEBUG) $(INC) -L${CLD3} -L${PROTOBUF}
LFLAGS = -Wall -Werror -std=c++14 -lstdc++fs  $(DEBUG) $(INC) -L${CLD3} -L${PROTOBUF_LIBS}
LIBS = -lcld3_full -lprotobuf

test_cld3_cli : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) -o test_cld3_cli

cld3_cli_test.o : cld3_cli_test.cc
	$(CC) $(CXXFLAGS) $(LIBS) cld3_cli_test.cc

clean:
	rm -rf *.o *~ test_cld3_cli test_cld3_cli.dSYM
