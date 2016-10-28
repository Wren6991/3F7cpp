CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS= -g -Wall
LDFLAGS=
LDLIBS=

APPNAME=camzip
SRCS=main.cpp alphabet.cpp code.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all:$(APPNAME)

run: all
	./$(APPNAME)

$(APPNAME): $(OBJS)
	$(CXX) $(LDFLAGS) -o $(APPNAME) $(OBJS) $(LDLIBS)

clean:
	$(RM) $(OBJS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

include .depend