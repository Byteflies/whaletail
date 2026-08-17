# whaletail — command line utilities for WhaleTeq signal generators
#
#   whalepulse    SECG 4.0 single channel ECG system (SDK 5.9.1)

CXX      = g++
CPPFLAGS = -Isrc
CXXFLAGS = -O2 -Wall -Wextra

# The SDK ships one prebuilt library per architecture; pick the one that
# matches the build host.
UNAME_M := $(shell uname -m)
ifeq ($(UNAME_M),x86_64)
  SECG_LIB = secgx64
else ifneq (,$(filter $(UNAME_M),aarch64 arm64))
  SECG_LIB = secgrpi64
else ifneq (,$(filter $(UNAME_M),armv7l armv6l))
  SECG_LIB = secgrpi32
else ifneq (,$(filter $(UNAME_M),i686 i386))
  SECG_LIB = secgx86
else
  $(error Unsupported architecture $(UNAME_M))
endif

SECG_DIR    = sdk/secg
SECG_LIBDIR = $(SECG_DIR)/linux

# Device agnostic helpers, shared by every binary. These must not include an
# SDK header: the WhaleTeq SDKs export colliding symbol names and define
# clashing macros, so each device needs its own main and its own link.
COMMON_OBJS = src/argparse.o src/run.o

all: whalepulse

# DT_RUNPATH lets the binary find its SDK library relative to itself, so the
# repository can be copied to a target as-is with nothing installed system wide.
whalepulse: src/ecg_main.o $(COMMON_OBJS)
	$(CXX) -o $@ $^ -L$(SECG_LIBDIR) -l$(SECG_LIB) \
	    -Wl,--enable-new-dtags,-rpath,'$$ORIGIN/$(SECG_LIBDIR)'

src/ecg_main.o: CPPFLAGS += -DLINUX_GCC -I$(SECG_DIR)
src/ecg_main.o: src/ecg_main.c src/argparse.h src/run.h $(SECG_DIR)/WhaleTeqSECG_SDK.h

src/argparse.o: src/argparse.c src/argparse.h
src/run.o: src/run.c src/run.h

# The SDK headers are C++ (extern "C" blocks, bare bool), so the sources are
# compiled as C++ even though they are written in C style.
src/%.o: src/%.c
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f src/*.o whalepulse

.PHONY: all clean
