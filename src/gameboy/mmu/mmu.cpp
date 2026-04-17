// #include "mmu.h"

// #include "../memory/memory.h"

// namespace GameBoy {
// instead of CPU -> ppu/ram/cart...
//
// CPU → MMU → (PPU, Timer, Cart, RAM)
//
// 0000	3FFF	16 KiB ROM bank 00	From cartridge, usually a fixed bank
// 4000	7FFF	16 KiB ROM Bank 01–NN	From cartridge, switchable bank
// 8000	9FFF	8 KiB Video RAM (VRAM)	In CGB mode, switchable bank 0/1
// A000	BFFF	8 KiB External RAM	From cartridge, switchable bank if any
// C000	CFFF	4 KiB Work RAM (WRAM)
// D000	DFFF	4 KiB Work RAM (WRAM)	In CGB mode, switchable bank 1–7
// E000	FDFF	Echo RAM (mirror of C000–DDFF)
// FE00	FE9F	Object attribute memory (OAM)
// FEA0	FEFF	Not Usable	Nintendo says use of this area is prohibited.
// FF00	FF7F	I/O Registers
// FF80	FFFE	High RAM (HRAM)
// FFFF	FFFF    IE

// uint8_t read(uint16_t addr) {
//     if (addr < 0x3FFF) {
//         return cart
//     }
// }
