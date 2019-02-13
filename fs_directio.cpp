#include <iostream>
#include <chrono>
#include <algorithm>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>

#include <sys/stat.h>

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


void test(char *filepath, int num_mb, bool direct_io) {
    void *buf;
    int r_sz = num_mb * (1 << 20);
    int flag = O_RDONLY;
    if (direct_io) {
        flag |= O_DIRECT;
    }
    if (posix_memalign(&buf, getBlockSize(filepath), r_sz) == -1) {
        std::cerr << "Align Error: " << strerror(errno) << std::endl;
        exit(1);
    }
    auto start = std::chrono::system_clock::now();
    auto fd = open(filepath, flag);
    if (fd == -1) {
        std::cerr << "Open Error: " << strerror(errno) << std::endl;
        exit(1);
    }
//    lseek64(fd, 0, SEEK_SET);
    if (read(fd, buf, r_sz) == -1) {
        std::cerr << "Read Error: " << strerror(errno) << std::endl;
        exit(1);
    }
    if (close(fd) == -1) {
        std::cerr << "Close Error: " << strerror(errno) << std::endl;
        exit(1);
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    // Print columns: size,direct,time
    std::cout << num_mb << ","
              << std::boolalpha << direct_io << ","
              << std::fixed << elapsed_seconds.count() * 1000 << std::endl;
    free(buf);

//    int r_sz = num_mb * (1 << 20);
//    int flag = O_RDONLY;
//    if (direct_io) {
//        flag |= O_DIRECT | O_SYNC;
//    }
//    // Allocate the memory buffer.
//    auto buf = (unsigned char *) malloc(sizeof(char) * r_sz);
//    if (buf == nullptr) {
//        std::cerr << "Malloc Error: " << strerror(errno) << std::endl;
//        exit(1);
//    }
//    // Start timing.
//    auto start = std::chrono::system_clock::now();
//    // Open the file.
//    auto fd = open(filepath, flag);
//    if (fd < 0) {
//        std::cerr << "Open Error: " << strerror(errno) << std::endl;
//        exit(1);
//    }
//    // Read from the file.
//    lseek64(fd, 0, SEEK_SET);
//    if (read(fd, buf, r_sz) == -1) {
//        std::cerr << "Read Error: " << strerror(errno) << std::endl;
//        exit(1);
//    }
//    // Close the file.
//    if (close(fd) == -1) {
//        std::cerr << "Close Error: " << strerror(errno) << std::endl;
//        exit(1);
//    }
//    auto end = std::chrono::system_clock::now();
//    std::chrono::duration<double> elapsed_seconds = end - start;
//    // Print columns: size,direct,time
//    std::cout << num_mb << ","
//              << std::boolalpha << direct_io << ","
//              << std::fixed << elapsed_seconds.count() * 1000 << std::endl;
//    free(buf);
}

// TODO: Run with `./fs_directio /data/2GB.file < ./fs_directio.testcase > ./fs_directio.csv`.
int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: fs_directio <data-file-path>" << std::endl;
        return 1;
    }
    // Parse arguments.
    char *filepath = argv[1];
    int i = 0;
    std::cin >> i;

    int read_sz_arr[i];
    for (int j = 0; j < i; j++) {
        int tmp = 0;
        std::cin >> tmp;
        read_sz_arr[j] = tmp;
    }

    if (i > 0) {
        std::cout << "size,direct,time" << std::endl;
        for (int j = 0; j < i; j++) {
            test(filepath, read_sz_arr[j], true);
            test(filepath, read_sz_arr[j], false);
        }
    }
    return 0;
}
