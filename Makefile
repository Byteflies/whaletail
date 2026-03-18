CXX=g++
CFLAGS=-DLINUX_GCC
OBJS=main.o
SDKDIR=sdk
LIBARM64=secgrpi64
LIBX64=secgx64
LIBARM32=secgrpi32
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
LDFLAGS=-Wl,--enable-new-dtags,-rpath,'$$ORIGIN/$(SDKDIR)' -L$(SDKDIR)
BIN=whaletail
DEPS=$(SDKDIR)/WhaleTeqSECG_SDK.h

all: $(BIN)

$(BIN): $(OBJS) $(DEPS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS) -l$(LIB) $(CFLAGS)

$(OBJS): %.o: %.c $(DEPS)
	$(CXX) -c -o $@ $< $(CFLAGS)
