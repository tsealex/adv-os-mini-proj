#include <iostream>
//#include "cache.h"
#include <chrono>
#include <ctime>
#include <stdlib.h>     /* srand, rand */
#include <algorithm>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

char *getCmdOption(char **begin, char **end, const std::string &option) {
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end) {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char **begin, char **end, const std::string &option) {
    return std::find(begin, end, option) != end;
}

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

//// Usage: main <file-path>
//// TODO: Clear page cache before running this
////       sync; echo 3 > /proc/sys/vm/drop_caches
//int main(int argc, char *argv[]) {
//    if (argc < 2) {
//        std::cout << "Usage: main <records-file-path>" << std::endl;
//        return 1;
//    }
//    char *file_path = argv[1];
//    bool direct = false;
//    int num_records_to_read = 5000; //240000;
//    int seed = 1;
//    // Parse arguments.
//    // Direct IO?
//    if (cmdOptionExists(argv, argv + argc, "-d")) {
//        direct = true;
//    }
//    char *num_records_to_read_str = getCmdOption(argv, argv + argc, "-n");
//    if (num_records_to_read_str != 0) {
//        num_records_to_read = atoi(num_records_to_read_str);
//    }
//
//    char *seed_str = getCmdOption(argv, argv + argc, "-s");
//    if (seed_str != 0) {
//        seed = atoi(seed_str);
//    }
//    std::cout << "DirectIO: " << direct << "; NumRecords: " << num_records_to_read << "; Seed: " << seed << std::endl;
//    // 1 KB per block, 200000 max # of nodes.
//    auto f_cache = FileCache((char *) file_path, num_records_to_read, 1 << 10, direct);
////    uint64_t record_id = 250000 * 64;
////    auto v = f_cache.Read(16 * record_id + 8);
//    srand(seed);
//    int repeat = 2;
//    uint64_t sum = 0;
//    int idx_arr[num_records_to_read];
//    for (int i = 0; i < num_records_to_read; i++) {
//        int j = rand() % 2000000;
//        idx_arr[i] = j;
////        std::cout << j << " " << idx_arr[i] << std::endl;
////        sum += idx_arr[i];
//    }
////    std::cout << sum << std::endl;
//
//    auto start = std::chrono::system_clock::now();
//    for (int k = 0; k < repeat; k++) {
//        // Repeat the following process 10 times.
//        for (int i = 0; i < num_records_to_read; i++) {
//            // Only read the first record of each cache block.
//            // Current cache block is 16 blocks from the previous one.
//            uint64_t record_id = idx_arr[i] * 64;
//            auto v = to_bytes(f_cache.Read(16 * record_id + 8));
//            sum = (sum + v) % 4294967296;
////            std::cout << to_bytes(f_cache.Read(16 * record_id)) << ": " << v << std::endl;
////            std::cout << sum << std::endl;
//        }
//    }
//    auto end = std::chrono::system_clock::now();
//    std::cout << sum << std::endl;
////    std::cout << to_bytes(f_cache.Read(16 * record_id)) << ": " << to_bytes(v) << std::endl;
////    std::cout << "Hello, World!" << std::endl;
//    std::chrono::duration<double> elapsed_seconds = end - start;
//    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
//
//    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
//    return 0;
//}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: main <records-file-path>" << std::endl;
        return 1;
    }
    // Parse arguments.
    char *file_path = argv[1];
    // File mode.
    bool flag = O_RDONLY;
    // How many bytes should be read from the file and written to memory. 1GB by default.
    int read_sz = 1 << 30;
    // How many times this process should repeat.
    int repeat = 1;
    if (cmdOptionExists(argv, argv + argc, "-d")) {
        flag |= O_DIRECT;
    }
    char *read_sz_str = getCmdOption(argv, argv + argc, "-b");
    if (read_sz_str != 0) {
        read_sz = atoi(read_sz_str);
    }
    char *repeat_str = getCmdOption(argv, argv + argc, "-r");
    if (repeat_str != 0) {
        repeat = atoi(repeat_str);
    }
    // Main logic.
    // Allocate the memory buffer.
    auto mem_buf = (unsigned char *) malloc(read_sz);
    // Start timing.
    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < repeat; i++) {
        // Open the file.
        auto fd = open(file_path, flag);
        // Read from the file.
        read(fd, mem_buf, read_sz);
        // Close the file.
        close(fd);
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << mem_buf[0] << std::endl;
    std::cout << "time: " << elapsed_seconds.count() << "s\n";
    return 0;
}