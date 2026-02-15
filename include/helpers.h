#pragma once

#include <sstream>

namespace Gameboy {
    /*
     * Message helper for formatting one arugment.
     *
     * Examples:
     * msg("PC = ", pc);
     */
    template <typename T>
    static std::string msg(const char* prefix, T value) {
        std::ostringstream oss;
        oss << prefix << value;
        return oss.str();
    }

    /*
     * Message helper that formats one argue in the middle as well as the end.
     *
     * Examples:
     * msg("I am ", 21, " years and ", 3.0/4.0);
     * msg("Bank ", bank, " selected: ", romBank);
     */
    template <typename A, typename B>
    static std::string msg(const char* prefix, A a,
                           const char* mid, B b) {
        std::ostringstream oss;
        oss << prefix << a << mid << b;
        return oss.str();
    }

}
