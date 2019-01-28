#
# Created 190126 lynnl
#

CC=clang
CPPFLAGS+=-D__TARGET_OS__=\"$(shell uname -m)-apple-darwin_$(shell uname -r)\" \
	-D__TS__=\"$(shell date +'%Y/%m/%d\ %H:%M:%S%z')\"
CFLAGS+=-std=c99 -Wall -Wextra -Werror \
	-arch x86_64 -arch i386 \
	-mmacosx-version-min=10.4
SOURCES=$(wildcard *.c)
EXECUTABLE=daemon
RM=rm

all: debug

release: $(SOURCES)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $(EXECUTABLE)

debug: CPPFLAGS += -g -DDEBUG
debug: release

clean:
	$(RM) -rf *.o *.dSYM $(EXECUTABLE)

.PHONY: all debug release clean

