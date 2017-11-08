
#include "fstream_helper.h"

#include <ios>

namespace utils {

    int64_t size(std::ifstream& ifs) {
        ifs.seekg(std::streamoff(0), std::ios::end);
        return ifs.tellg();
    }

    void seek(std::ifstream& ifs, int64_t pos) {
        ifs.clear();
        ifs.seekg(std::streampos(pos));
    }

}
