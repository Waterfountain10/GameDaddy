//
// Created by William Kiem Lafond on 2025-09-17.
//

#ifndef ROM_VALIDATION_H
#define ROM_VALIDATION_H
#include <vector>

namespace Gameboy {

class RomValidation {
public:
    RomValidation();

    struct RomValidationResult {
        bool ok = false;
        std::vector<std::string> errors;


    };
private:

};
}

#endif //ROM_VALIDATION_H
