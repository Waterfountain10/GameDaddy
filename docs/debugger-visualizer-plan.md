# GameDaddy debugger visualizer plan

Issue: #49

## Goal

`scripts/debug.sh` should launch a GameDaddy-specific SDL visual debugger/profiler.

The normal emulator window stays available through `scripts/run.sh`. Debug mode gets its own SDL window with the Game Boy viewport on the left and a dense debugger corridor on the right.

## Layout direction

```text
┌ Game Boy viewport ───────────────────────────────┬ CPU / controls ──────────┐
│                                                  │ A F B C D E H L           │
│  160x144 scaled emulator output                  │ PC SP flags IME           │
│                                                  │ q quit s step f frame     │
├ Memory map / selected region ────────────────────┤ r run/pause               │
│ ROM VRAM CART RAM WRAM OAM IO HRAM IE            ├ Instructions ─────────────┤
│                                                  │ 00FA LD A 01              │
│                                                  │ 00FC LDH FF50 A           │
│                                                  │ 00FE JP 0100              │
│                                                  │ >>> 0100 NOP              │
│                                                  │ 0101 LD BC D16            │
│                                                  ├ Profiler ─────────────────┤
│                                                  │ FPS frames cycles hot ops  │
└──────────────────────────────────────────────────┴───────────────────────────┘
```

## First PR scope

Keep this additive and easy to review:

1. Add a `gamedaddy_debugger` executable.
2. Update `scripts/debug.sh` to launch it.
3. Preserve LLDB through `scripts/lldb.sh`.
4. Draw a first SDL debugger shell with panels.
5. Add compact instruction/profiler placeholders.

Do **not** rewrite CPU, Platform, Bus, PPU, or ROM loading in the first debugger PR.

## Later phases

- Wire real CPU register snapshots.
- Add previous/current/next instruction disassembly around PC.
- Add memory jumping and watchpoints.
- Add breakpoints and run-until-PC.
- Add bus read/write traces.
- Add PPU/timer/interrupt/joypad panels as those subsystems mature.
- Add htop-style profiler counters: frame time, cycles, instructions/sec, hot opcodes, bus reads/writes by region.

## Rule of thumb

The debugger renders read-only snapshots from the emulator. It should not directly mutate private emulator internals.
