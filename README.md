# whaletail

Minimal command line utilities for driving WhaleTeq signal generators.

| Binary | Device | Vendor SDK |
| --- | --- | --- |
| `whalepulse` | SECG 4.0 single channel ECG system | 5.9.1 (Linux) |
| `whalewave` | SEEG 100 / 100E EEG system | 1.6.4 (Linux) |

Each device gets its own executable. The WhaleTeq SDKs export more than thirty
colliding symbol names (`SetAmplitude`, `SetFrequency`, `SetOutputFunc`, ...)
and define the same macros and enums with different values, so the two cannot be
linked into one binary or included into one translation unit. Only one device is
expected to be attached at a time; pick the binary that matches it.

## Build

```sh
make                # both binaries
make whalepulse     # just one
```

The correct prebuilt SDK library is selected from `uname -m` (x86-64, i686,
aarch64, armv7l). Each executable records a `DT_RUNPATH` of
`$ORIGIN/sdk/<device>/linux`, so the whole directory can be copied to a target
and run in place with nothing installed system wide.

Requires `g++`. The vendor headers are C++ (`extern "C"` blocks, bare `bool`),
so the sources are compiled as C++ even though they are written in C style.

## Usage

Both binaries configure the device, start generating, and keep generating until
interrupted, at which point the device is closed cleanly.

**Watch the amplitude unit: `whalepulse` takes mV and `whalewave` takes µV.**

### whalepulse (ECG)

```
whalepulse <wave_shape> <frequency> <amplitude> [pace <bpm> <amplitude> <duration>] [leads <lead>[,<lead>...]]
```

| Argument | Unit / values |
| --- | --- |
| `wave_shape` | `off`, `sine`, `triangle`, `square`, `rectanglepulse`, `trianglepulse`, `exponential`, `ecg2_27`, `iec227w`, `iec251w`, `jjg1041`, `jjg1041_hr`, `jjg_hysteresis`, `ecg_file` |
| `frequency` | Hz |
| `amplitude` | mV |
| `bpm` | beats per minute |
| pacing `amplitude` | mV |
| pacing `duration` | ms |
| `leads` | any of `ra`/`r`, `la`/`l`, `ll`/`f`, `v1`–`v6`, comma separated; defaults to `la,v3` |

```sh
./whalepulse sine 1.0 1.0                       # 1 Hz, 1 mV sine on la and v3
./whalepulse sine 1.0 1.0 pace 60 1.0 0.5       # ...with a 60 bpm pacing pulse
./whalepulse sine 1.0 1.0 leads ra,la           # ...on ra and la instead
```

Both optional groups may be given in either order.

The SECG is a single channel system — the SDK exposes one shape, frequency and
amplitude for the whole device — so every enabled lead necessarily carries the
**identical** signal. Leads not listed are switched off explicitly. The default
`la,v3` is the selection this tool has always applied; pass `leads` to override
it.

### whalewave (EEG)

```
whalewave <wave_shape> <frequency> <amplitude> [channels <ch>[,<ch>...]]
```

| Argument | Unit / values |
| --- | --- |
| `wave_shape` | `off`, `sine`, `triangle`, `square`, `rectanglepulse`, `trianglepulse`, `hysteresis`, `playwaveform` |
| `frequency` | Hz, 0.05–500 (0.05–5 for the pulse shapes) |
| `amplitude` | µV, −2000 to 2000 |
| `channels` | any of `ch1`–`ch9`, comma separated; defaults to `ch1` |

```sh
./whalewave sine 10.0 100.0                    # 10 Hz, 100 µV sine on ch1
./whalewave sine 10.0 100.0 channels ch1,ch2   # ...on ch1 and ch2 at once
```

The device has one generator feeding a relay matrix, so every selected channel
carries the **identical** signal — which is what makes a two-channel test
possible. Channels not listed are switched off explicitly, so the selection does
not depend on how the device was last left.

Only `ch1`–`ch9` can output a waveform. The remaining breakout-box terminals,
Ch10–Ch22, are tied to ground (D15 pin 4) and are rejected as arguments.

The device is selected automatically with a 5 second connection timeout.

### Not yet exposed

`whalepulse`'s `ecg_file` and `whalewave`'s `playwaveform` replay a waveform
loaded from a file, but neither binary offers a way to load one, so both will
fail or emit nothing. `whalewave` also does not expose DC offset, pulse width,
baseline reset test, or frequency scanning.

## Device access

Both devices appear as `/dev/ttyACM0`, owned by group `dialout`. A failure to
initialise or connect usually means the calling user is not in that group:

```sh
sudo usermod -a -G dialout "$USER"   # log out and back in afterwards
```

The SEEG SDK additionally takes a lock file per port. If `whalewave` cannot
connect to a device that is definitely attached, clear any stale locks:

```sh
sudo rm -f /var/lock/whaleteq*
```

## Layout

```
src/
  ecg_main.c     whalepulse: CLI and SECG SDK calls
  eeg_main.c     whalewave: CLI and SEEG SDK calls
  argparse.[ch]  argument value and enum name parsing
  run.[ch]       idle loop, signal handling, device shutdown
sdk/secg/        vendored SECG SDK header and per-architecture libraries
sdk/seeg/        vendored SEEG SDK header and per-architecture libraries
```

`argparse` and `run` are shared by both binaries and must not include an SDK
header; everything device specific belongs in the corresponding `*_main.c`.
