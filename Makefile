CC=g++
CFLAGS=-DLINUX_GCC
OBJS=main.o
LIBARM64=sdk/libsecgrpi64.so
LIBX64=sdk/libsecgx64.so
LIBARM32=sdk/libsecgrpi32.so
# auto select lib according to host architecture
UNAME_M := $(shell uname -m)
ifeq ($(UNAME_M),x86_64)
	LIB=$(LIBX64)
else ifeq ($(UNAME_M),aarch64)
	LIB=$(LIBARM64)
else ifeq ($(UNAME_M),arm64)
	LIB=$(LIBARM64)
else ifeq ($(UNAME_M),armv7l)
	LIB=$(LIBARM32)
else ifeq ($(UNAME_M),armv6l)
	LIB=$(LIBARM32)
else
	$(error Unsupported architecture $(UNAME_M))
endif
BIN=whaletail
DEPS=sdk/WhaleTeqSECG_SDK.h

all: $(BIN)

$(BIN): $(OBJS) $(DEPS)
	$(CC) -o $@ $(OBJS) $(LIB) $(CFLAGS)
