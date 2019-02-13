//
// Created by Alex on 2/9/2019.
//
#include <iostream>
#include <chrono>
#include <algorithm>
#include <fcntl.h>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>

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

size_t getBlockSize(const char *filepath) {
    struct stat fstat;
    stat(filepath, &fstat);
    return (size_t) fstat.st_blksize;
}

//void test_random_write(char *filepath, int write_range, bool direct_io) {
//    auto wr_data = (char *) malloc(sizeof(char) * 1024); // 1KB
//    int flag = O_WRONLY;
//    if (direct_io) {
//        flag |= O_DIRECT;
//    }
//    // Start timing.
//    auto start = std::chrono::system_clock::now();
//    // Open the file for writing.
//    auto fd = open(filepath, flag);
//    // Start writing.
//    int64_t bound = write_range * (1 << 20); // in MBs.
//    for (int i = 0; i < 10 * write_range; i++) {
//        int64_t pos = rand() % bound;
//        lseek64(fd, pos, SEEK_SET);
//        write(fd, &wr_data, 1);
//    }
//    // Close the file.
//    close(fd);
//    auto end = std::chrono::system_clock::now();
//    std::chrono::duration<double> elapsed_seconds = end - start;
//    // Print columns: size,direct,time
//    std::cout << write_range << ","
//              << std::boolalpha << direct_io << ","
//              << std::fixed << elapsed_seconds.count() * 1000 << std::endl;
//    free(wr_data);
//}

void test_repeated_write(char *filepath, int num_writes, bool direct_io) {
    void *buf;
    uint32_t wr_sz = 10 * (1 << 20); // 10MB
    int flag = O_RDWR;
    if (direct_io) {
        flag |= O_DIRECT | O_SYNC;
    }
    if (posix_memalign(&buf, getBlockSize(filepath), wr_sz) == -1) {
        std::cerr << "Align Error: " << strerror(errno) << std::endl;
        exit(1);
    }
    // Start timing.
    auto start = std::chrono::system_clock::now();
    // Open the file for writing.
    auto fd = open(filepath, flag);
    if (fd < 0) {
        std::cerr << "Open Error: " << strerror(errno) << std::endl;
        exit(1);
    }
    // Start writing.
    for (int i = 0; i < num_writes; i++) {
        lseek64(fd, 0, SEEK_SET);
        if (read(fd, buf, wr_sz) == -1) {
            std::cerr << "Read Error: " << strerror(errno) << std::endl;
            exit(1);
        }
        lseek64(fd, 0, SEEK_SET);
        if (write(fd, buf, wr_sz) == -1) {
            std::cerr << "Write Error: " << strerror(errno) << std::endl;
            exit(1);
        }
    }
    // Close the file.
    auto b = close(fd);
    if (b == -1) {
        std::cerr << "Close Error: " << strerror(errno) << std::endl;
        exit(1);
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    // Print columns: num_wr,direct,time
    std::cout << num_writes << ","
              << std::boolalpha << direct_io << ","
              << std::fixed << elapsed_seconds.count() * 1000 << std::endl;
    free(buf);
}

// TODO: Run with `./fs_default /data/2GB.file < ./fs_default.testcase > ./fs_default_rand_wr.csv`.
int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: fs_default <data-file-path>" << std::endl;
        return 1;
    }
    // Parse arguments.
    char *filepath = argv[1];
    int i = 0;
    std::cin >> i;

    int arg_arr[i];
    for (int j = 0; j < i; j++) {
        int tmp = 0;
        std::cin >> tmp;
        arg_arr[j] = tmp;
    }

    if (i > 0) {
        // num_wr means the number of 1MB writes performed on the beginning portion of the file.
        std::cout << "num_wr,direct,time" << std::endl;
        for (int j = 0; j < i; j++) {
//            int64_t seed = time(NULL);
//            srand(seed);
//            test_random_write(filepath, arg_arr[j], false);
//            srand(seed);
//            test_random_write(filepath, arg_arr[j], true);
            test_repeated_write(filepath, arg_arr[j], false);
            test_repeated_write(filepath, arg_arr[j], true);
        }
    }
    return 0;
}