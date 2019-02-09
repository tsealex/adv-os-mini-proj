#include <iostream>
#include "cache.h"


uint64_t to_bytes(const unsigned char *array) {
    return static_cast<uint64_t>(array[0]) |
           static_cast<uint64_t>(array[1]) << 8 |
           static_cast<uint64_t>(array[2]) << 16 |
           static_cast<uint64_t>(array[3]) << 24 |
           static_cast<uint64_t>(array[4]) << 32 |
           static_cast<uint64_t>(array[5]) << 40 |
           static_cast<uint64_t>(array[6]) << 48 |
           static_cast<uint64_t>(array[7]) << 56;
}

int main() {
    auto f_cache = FileCache((char *) "/home/alex/Projects/CS736/mini-project/records", 10000, 1 << 10, false);
    uint64_t record_id = 155566;
    auto v = f_cache.Read(16 * record_id + 8);
    std::cout << to_bytes(f_cache.Read(16 * record_id)) << ": " << to_bytes(v) << std::endl;
//    std::cout << "Hello, World!" << std::endl;
    return 0;
}