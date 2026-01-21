# GameDaddy (ongoing)

GameDaddy is an ongoing Game Boy emulator project written in modern C++. It’s not finished, and that’s intentional — this is about understanding hardware behavior and emulator architecture, not shipping a polished emulator (yet).

- GameDaddy emulates the original Game Boy’s core: a Sharp SM83-derived 8-bit CPU running at ~4.19 MHz with a 16-bit address space and memory map that includes ROM, RAM, VRAM, I/O registers, and more. See the [Game Boy Pan Docs](https://gbdev.io/pandocs/) for complete specs.  
- The hardware core (CPU, memory, timers, graphics and audio state) is separate from the platform layer (SDL for window, input, audio output, and timing).  
- Instruction decoding uses table-driven dispatch instead of giant switch statements to match how the real CPU’s opcode structure works and to keep control flow predictable.  
- Graphics eventually needs to produce the Game Boy’s 160×144 LCD output with tile maps, background/window layers, and sprites; this is partially started but incomplete. The LCD control and scanline timing are described in the [Pan Docs LCDC section](https://gbdev.io/pandocs/LCDC.html).  
- Audio emulation is a stubbed interpretation of the Game Boy’s APU with four channels; the full synced behavior is detailed in the [Pan Docs audio chapter](https://gbdev.io/pandocs/Audio.html).  
- Many pieces are still works in progress: full PPU timing, complete opcode coverage, banked memory / cartridge mappers, interrupt handling, and cycle-accurate synchronization.

This project should be read as a structured emulator skeleton and a learning playground, not a finished product.


