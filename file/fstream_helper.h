
#ifndef UTILS_FILE_FSTREAM_HELPER_H_
#define UTILS_FILE_FSTREAM_HELPER_H_

#include <fstream>

namespace utils {

    int64_t size(std::ifstream& ifs);
    void seek(std::ifstream& ifs, int64_t pos);
}

#endif
