#include <iostream>
#include <chrono>
#include <algorithm>
#include <fcntl.h>
#include <cstring>
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


void test(char *filepath, int num_mb, bool direct_io) {// Allocate the memory buffer.
    int read_sz = num_mb * (1 << 20);
    int flag = O_RDONLY;
    if (direct_io) {
        flag |= O_DIRECT;
    }
    auto mem_buf = (unsigned char *) malloc(read_sz);
    // Start timing.
    auto start = std::chrono::system_clock::now();
    // Open the file.
    auto fd = open(filepath, flag);
    // Read from the file.
    auto sz = read(fd, mem_buf, read_sz);
    bool tmp = mem_buf[0] == mem_buf[sz - 1];
    // Close the file.
    close(fd);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    // Print columns: size,direct,time
    std::cout << num_mb << ","
              << std::boolalpha << direct_io << ","
              << std::fixed << elapsed_seconds.count() * 1000 << std::endl;
    free(mem_buf);
}

// TODO: Generate the data file with `truncate -s 2g /data/2GB.file`
// TODO: Build with `g++ ./directio.cpp -o directio`
// TODO: Run with `./directio /data/2GB.file < ./directio.testcase > ./directio.csv`.
int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: main <records-file-path>" << std::endl;
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
