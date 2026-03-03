
#include "cart.h"
#include "cartridge/mbc/mbc.h"
#include "cartridge/rom/rom.h"

namespace Cartridge {

Cart::Cart(std::vector<uint8_t> rom) : rom_(std::move(rom)) {
    cart_type_ = rom_.at(OFF_CARTRIDGE_T);
    rom_size_code_ = rom_.at(OFF_ROM_SIZE);
    ram_size_code_ = rom_.at(OFF_RAM_SIZE);

    alloc_ram_();
    attach_mbc_();
}

// Forward function to mbc's read.
uint8_t Cart::call_read(uint16_t addr) {
    return mbc_->read(addr);
}

// Forward function to mbc's write.
void Cart::call_write(uint16_t addr, uint8_t value) {
    mbc_->write(addr, value);
}

// Resize ram_ into "ram size", specified with ram_size_code_ at 0x149.
void Cart::alloc_ram_() {
    ram_.resize(ram_size_bytes(ram_size_code_), 0xFF); // most hardware inits with high
}

// Forward function to mbc's read.
void Cart::attach_mbc_() {
    switch (cart_type_) {
        case 0x00: // ROM-ONLY
            mbc_ = std::make_unique<RomOnly>(rom_, ram_);
            break;
        case 0x01: // MBC 1
            mbc_ = std::make_unique<MBC1>(rom_, ram_);
            break;
        default:
            throw std::runtime_error("Unsupported cartridge type: " + std::to_string(cart_type_));
    }
}

} // namespace Cartridge
