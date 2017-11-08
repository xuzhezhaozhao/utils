
#include "hash.h"

namespace utils {

    uint32_t hash(const std::string& str) {
        uint32_t h = 2166136261;
        for (size_t i = 0; i < str.size(); i++) {
            h = h ^ uint32_t(str[i]);
            h = h * 16777619;
        }
        return h;
    }
}
