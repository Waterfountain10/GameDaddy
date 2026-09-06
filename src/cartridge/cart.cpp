#include "cart.h"
#include "cartridge/mbc/mbc.h"
#include "cartridge/rom/rom.h"

namespace Cartridge {

Cart::Cart(std::vector<uint8_t> rom) : rom_(std::move(rom)) {
    cart_type_     = rom_.at(OFF_CARTRIDGE_T);
    rom_size_code_ = rom_.at(OFF_ROM_SIZE);
    ram_size_code_ = rom_.at(OFF_RAM_SIZE);

    alloc_ram_();
    attach_mbc_();
}

uint8_t Cart::read(uint16_t addr) {
    return mbc_->read(addr);
}

void Cart::write(uint16_t addr, uint8_t value) {
    mbc_->write(addr, value);
}

void Cart::alloc_ram_() {
    ram_.resize(ram_size_bytes(ram_size_code_), 0xFF); // most hardware inits with high
}

void Cart::attach_mbc_() {
    switch (cart_type_) {
    case 0x00: // ROM-ONLY
        mbc_ = std::make_unique<RomOnly>(rom_, ram_);
        break;
    case 0x01: // MBC 1
    case 0x02: // MBC 1 + RAM
    case 0x03: // MBC 1 + RAM + BATTERY
        mbc_ = std::make_unique<MBC1>(rom_, ram_);
        break;
    case 0x05: // MBC 2
    case 0x06: // MBC 2 + BATTERY
        mbc_ = std::make_unique<MBC2>(rom_);
        break;
    case 0x0F: // MBC 3 + TIMER + BATTERY
    case 0x10: // MBC 3 + TIMER + RAM + BATTERY
    case 0x11: // MBC 3
    case 0x12: // MBC 3 + RAM
    case 0x13: // MBC 3 + RAM + BATTERY
        mbc_ = std::make_unique<MBC3>(rom_, ram_);
        break;
    case 0x19: // MBC 5
    case 0x1A: // MBC 5 + RAM
    case 0x1B: // MBC 5 + RAM + BATTERY
    case 0x1C: // MBC 5 + RUMBLE
    case 0x1D: // MBC 5 + RUMBLE + RAM
    case 0x1E: // MBC 5 + RUMBLE + RAM + BATTERY
        mbc_ = std::make_unique<MBC5>(rom_, ram_);
        break;
    default:
        throw std::runtime_error("Unsupported cartridge type: " + std::to_string(cart_type_));
    }
}

} // namespace Cartridge
