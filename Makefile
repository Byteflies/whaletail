CC=g++
CFLAGS=-DLINUX_GCC
OBJS=main.o
LIBARM64=sdk/libsecgrpi64.so
LIBX64=sdk/libsecgx64.so
# auto select lib according to host architecture
UNAME_M := $(shell uname -m)
ifeq ($(UNAME_M),x86_64)
	LIB=$(LIBX64)
else
	LIB=$(LIBARM64)
endif
BIN=whaletail
DEPS=sdk/WhaleTeqSECG_SDK.h

all: $(BIN)

$(BIN): $(OBJS) $(DEPS)
	$(CC) -o $@ $(OBJS) $(LIB) $(CFLAGS)
