CC = g++
CFLAGS = -g -Wall -O0
INCLUDES = 
LIBS = -lcrypto

C_SRCS = $(wildcard *.c)
C_OBJS = $(C_SRCS:.c=.o)

CC_SRCS = $(wildcard *.cc)
CC_OBJS = $(CC_SRCS:.cc=.o)

CPP_SRCS = $(wildcard *.cpp)
CPP_OBJS = $(CPP_SRCS:.cpp=.o)

SRCS = ${C_SRCS} ${CC_SRCS} ${CPP_SRCS}
OBJS = ${C_OBJS} ${CC_OBJS} ${CPP_OBJS}

#.c.o:
#	${CC} ${CFLAGS} ${INCLUDES} -o $@ -c $< 

all : $(OBJS) pw b64

pw : pw.cc
	$(CC) $(CFLAGS) -lcrypto -o $@ $<
	./pw

b64 : b64.cc
	$(CC) $(CFLAGS) $(LIBS) -o $@ $<
	./b64

clean:
	rm ${OBJS}

.PHONY: clean all
