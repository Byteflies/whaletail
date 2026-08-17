# whaletail

Minimal command line utilities for driving WhaleTeq signal generators.

| Binary | Device | Vendor SDK |
| --- | --- | --- |
| `whalepulse` | SECG 4.0 single channel ECG system | 5.9.1 (Linux) |

Each device gets its own executable. The WhaleTeq SDKs export more than thirty
colliding symbol names (`SetAmplitude`, `SetFrequency`, `SetOutputFunc`, ...)
and define the same macros and enums with different values, so two of them
cannot be linked into one binary or included into one translation unit. Only one
device is expected to be attached at a time; pick the binary that matches it.

## Build

```sh
make
```

The correct prebuilt SDK library is selected from `uname -m` (x86-64, i686,
aarch64, armv7l). The executable records a `DT_RUNPATH` of
`$ORIGIN/sdk/secg/linux`, so the whole directory can be copied to a target and
run in place with nothing installed system wide.

Requires `g++`. The vendor headers are C++ (`extern "C"` blocks, bare `bool`),
so the sources are compiled as C++ even though they are written in C style.

## Usage

```
whalepulse <wave_shape> <frequency> <amplitude> [pace <bpm> <amplitude> <duration>]
```

| Argument | Unit |
| --- | --- |
| `wave_shape` | one of `off`, `sine`, `triangle`, `square`, `rectanglepulse`, `trianglepulse`, `exponential`, `ecg2_27`, `iec227w`, `iec251w`, `jjg1041`, `jjg1041_hr`, `jjg_hysteresis`, `ecg_file` |
| `frequency` | Hz |
| `amplitude` | mV |
| `bpm` | beats per minute |
| pacing `amplitude` | mV |
| pacing `duration` | ms |

Output continues until the process is interrupted, at which point the device is
closed cleanly:

```sh
./whalepulse sine 1.0 1.0                    # 1 Hz, 1 mV sine
./whalepulse sine 1.0 1.0 pace 60 1.0 0.5    # ...with a 60 bpm pacing pulse
```

## Device access

The device appears as `/dev/ttyACM0`, owned by group `dialout`. `InitSECG
failed` usually means the calling user is not in that group:

```sh
sudo usermod -a -G dialout "$USER"   # log out and back in afterwards
```

## Layout

```
src/
  ecg_main.c     whalepulse: CLI and SECG SDK calls
  argparse.[ch]  argument value and enum name parsing
  run.[ch]       idle loop, signal handling, device shutdown
sdk/secg/        vendored SECG SDK header and per-architecture libraries
```

`argparse` and `run` are shared by every binary and must not include an SDK
header; everything device specific belongs in the corresponding `*_main.c`.
