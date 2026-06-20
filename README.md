# BusproCore

Shared HDL-Buspro-style RS485 bus layer, factored out of the 4R library so
that every subdevice board sharing the same bus (4R relay board, 4Z input
board, and any future boards) uses **one** frame codec / transport /
dispatch implementation instead of duplicating it per device.

This matters specifically because of the unverified-wire-format situation:
when you capture real HDL traffic and fix the CRC/sync/length convention,
you fix it **once**, here, and every device library that depends on
BusproCore picks up the fix automatically.

## ⚠️ Wire frame format is still a placeholder

See `src/BusproFrame.cpp` for the exact placeholder layout and the list of
`TODO_VERIFY_HDL` items. Do not deploy on a bus with real HDL Buspro
hardware until verified against real captures (see `docs/CAPTURE_TEMPLATE.md`
in the 4R library, which applies equally here).

## What's in here

```
src/BusproFrame.h/.cpp     Decoded frame struct + encode/decode + op-codes
src/BusproTransport.h/.cpp Shared-serial byte handling, RS485 DE/RE, address filtering
src/BusproDevice.h          Generic op-code dispatch table
```

Device-specific logic (relay control, scene tables, input debouncing, etc.)
lives in the separate device libraries (`4R`, `4Z`, ...), each of which
depends on this library and only adds its own handlers + op-code bindings.

## Op-code registry

Keeping all op-codes in one place (`BusproFrame.h`, `namespace BusproOp`)
avoids different device libraries accidentally colliding on the same
op-code number. When adding a new device library, add its op-codes here
rather than defining them locally.

| Op-code | Name | Device | Status |
|---|---|---|---|
| 0x0002 | SCENE_CONTROL | 4R | confirmed by user spec |
| 0x0031 | SINGLE_CHANNEL_CONTROL | 4R | TODO_VERIFY_HDL |
| 0x0032 | SINGLE_CHANNEL_RESPONSE | 4R | TODO_VERIFY_HDL |
| 0x0033 | READ_STATUS_REQUEST | 4R | TODO_VERIFY_HDL |
| 0x0034 | READ_STATUS_RESPONSE | 4R | TODO_VERIFY_HDL |
| 0x0041 | INPUT_STATUS_PUSH | 4Z | TODO_VERIFY_HDL |
| 0x0042 | INPUT_STATUS_REQUEST | 4Z | TODO_VERIFY_HDL |
| 0x0043 | INPUT_STATUS_RESPONSE | 4Z | TODO_VERIFY_HDL |
| 0x0051 | TEMP_STATUS_PUSH | 4T | TODO_VERIFY_HDL |
| 0x0052 | TEMP_STATUS_REQUEST | 4T | TODO_VERIFY_HDL |
| 0x0053 | TEMP_STATUS_RESPONSE | 4T | TODO_VERIFY_HDL |

## Installing alongside device libraries

Arduino IDE: install both `BusproCore` and `4R`/`4Z` into your libraries
folder; the `#include <BusproFrame.h>` etc. in the device libraries will
resolve via the IDE's library search path.

PlatformIO: add both as `lib_deps` (e.g. via local `file://` paths, or a
private registry/git repo if you publish these), or place both under your
project's `lib/` directory -- PlatformIO's dependency resolution will pick
up BusproCore automatically since 4R/4Z's `library.json` declare it.
