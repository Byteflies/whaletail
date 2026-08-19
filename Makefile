# whaletail — command line utilities for WhaleTeq signal generators
#
#   whalepulse    SECG 4.0 single channel ECG system (SDK 5.9.1)
#   whalewave     SEEG 100 / 100E EEG system (SDK 1.6.4)

CXX      = g++
CPPFLAGS = -Isrc
CXXFLAGS = -O2 -Wall -Wextra

# Each SDK ships one prebuilt library per architecture; pick the ones that
# match the build host. Note that the SEEG library for 32 bit ARM is named
# "rpix86" despite not being an x86 build.
UNAME_M := $(shell uname -m)
ifeq ($(UNAME_M),x86_64)
  SECG_LIB = secgx64
  SEEG_LIB = seegx64
else ifneq (,$(filter $(UNAME_M),aarch64 arm64))
  SECG_LIB = secgrpi64
  SEEG_LIB = seegrpix64
else ifneq (,$(filter $(UNAME_M),armv7l armv6l))
  SECG_LIB = secgrpi32
  SEEG_LIB = seegrpix86
else ifneq (,$(filter $(UNAME_M),i686 i386))
  SECG_LIB = secgx86
  SEEG_LIB = seegx86
else
  $(error Unsupported architecture $(UNAME_M))
endif

SECG_DIR    = sdk/secg
SECG_LIBDIR = $(SECG_DIR)/linux
SEEG_DIR    = sdk/seeg
SEEG_LIBDIR = $(SEEG_DIR)/linux

# Device agnostic helpers, shared by every binary. These must not include an
# SDK header: the WhaleTeq SDKs export colliding symbol names and define
# clashing macros, so each device needs its own main and its own link.
COMMON_OBJS = src/argparse.o src/run.o

all: whalepulse whalewave

# DT_RUNPATH lets each binary find its SDK library relative to itself, so the
# repository can be copied to a target as-is with nothing installed system wide.
whalepulse: src/ecg_main.o $(COMMON_OBJS)
	$(CXX) -o $@ $^ -L$(SECG_LIBDIR) -l$(SECG_LIB) \
	    -Wl,--enable-new-dtags,-rpath,'$$ORIGIN/$(SECG_LIBDIR)'

whalewave: src/eeg_main.o $(COMMON_OBJS)
	$(CXX) -o $@ $^ -L$(SEEG_LIBDIR) -l$(SEEG_LIB) \
	    -Wl,--enable-new-dtags,-rpath,'$$ORIGIN/$(SEEG_LIBDIR)'

# The two SDKs want different feature macros, and their headers must never meet
# in one translation unit, so the include path is set per object rather than
# globally.
src/ecg_main.o: CPPFLAGS += -DLINUX_GCC -I$(SECG_DIR)
src/ecg_main.o: src/ecg_main.c src/argparse.h src/run.h $(SECG_DIR)/WhaleTeqSECG_SDK.h

src/eeg_main.o: CPPFLAGS += -DLINUX -I$(SEEG_DIR)
src/eeg_main.o: src/eeg_main.c src/argparse.h src/run.h $(SEEG_DIR)/SEEG100.h

src/argparse.o: src/argparse.c src/argparse.h
src/run.o: src/run.c src/run.h

# The SDK headers are C++ (extern "C" blocks, bare bool), so the sources are
# compiled as C++ even though they are written in C style.
src/%.o: src/%.c
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f src/*.o whalepulse whalewave

.PHONY: all clean
