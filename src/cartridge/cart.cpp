
#include "cart.h"
#include "cartridge/mbc/mbc.h"

namespace Cartridge {

static constexpr uint16_t OFF_CARTRIDGE_T = 0x0147;
static constexpr uint16_t OFF_ROM_SIZE    = 0x0148;
static constexpr uint16_t OFF_RAM_SIZE    = 0x0149;

Cart::Cart(std::vector<uint8_t> rom)
    : rom_(std::move(rom))
{
    cart_type_     = rom_.at(OFF_CARTRIDGE_T);
    rom_size_code_ = rom_.at(OFF_ROM_SIZE);
    ram_size_code_ = rom_.at(OFF_RAM_SIZE);

    alloc_ram_();
    attach_mbc_();
}

uint8_t Cart::call_read(uint16_t addr) {
    return mbc_->read(addr);
}

void Cart::call_write(uint16_t addr, uint8_t value) {
    mbc_->write(addr, value);
}

void Cart::attach_mbc_() {
    switch (cart_type_) {
        case 0x00: // ROM-ONLY
            mbc_ = std::make_unique<RomOnly>(rom_, ram_);
            break;
        case 0x01:
            mbc_ = std::make_unique<MBC1>(rom_, ram_);
            break;
        default:
            throw std::runtime_error("Unsupported cartridge type: " + std::to_string(cart_type_));
    }
}

}
