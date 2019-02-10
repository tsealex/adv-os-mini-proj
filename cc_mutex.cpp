//
// Created by Alex on 2/10/2019.
//
#include <iostream>
#include <chrono>
#include <algorithm>
#include <cstring>
#include <pthread.h>


uint32_t counter = 0;
uint32_t inc = 1;

auto target_num = 10000000; //(uint32_t) ~0;
pthread_spinlock_t spinlock;
pthread_mutex_t mutex;

void *spin_count(void *) {
    while (counter < target_num) {
        pthread_spin_lock(&spinlock);
//        if (counter < target_num) {
//            counter++;
//        }
        int added = 0;
        while (counter < target_num && added < inc) {
            counter++;
            added++;
        }
        pthread_spin_unlock(&spinlock);
    }
    pthread_exit(nullptr);
}


void *mutex_count(void *) {
    while (counter < target_num) {
        pthread_mutex_lock(&mutex);
//        if (counter < target_num) {
//            counter++;
//        }
        int added = 0;
        while (counter < target_num && added < inc) {
            counter++;
            added++;
        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(nullptr);
}

void count_test(int num_threads, bool spin) {
    pthread_t threads[num_threads];
    counter = 0;

    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], nullptr, (spin) ? spin_count : mutex_count, nullptr);
    }
    // Wait for the threads to finish.
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], nullptr);
    }

//    if (spin) {
//        while (counter < target_num) {
//            pthread_spin_lock(&spinlock);
//            pthread_spin_unlock(&spinlock);
//        }
//    } else {
//        while (counter < target_num) {
//            pthread_mutex_lock(&mutex);
//            pthread_mutex_lock(&mutex);
//        }
//    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    // Print columns: num_threads,spin,time,inc_per_itr
    std::cout << num_threads << ","
              << std::boolalpha << spin << ","
              << std::fixed << elapsed_seconds.count() * 1000 << ","
              << inc << std::endl;

}

// TODO: Run with `./cc_mutex < ./cc_mutex.testcase > ./cc_mutex.csv`.
int main(int argc, char *argv[]) {
    // Parse input.
    int n = 0;
    std::cin >> n;

    // Arguments are the number of threads and number of increments per iteration.
    int args[n][2];
    for (int j = 0; j < n; j++) {
        int tmp = 0;
        std::cin >> tmp;
        args[j][0] = tmp;
        std::cin >> tmp;
        args[j][1] = tmp;
    }

    pthread_spin_init(&spinlock, 0);
    pthread_mutex_init(&mutex, nullptr);

    if (n > 0) {
        std::cout << "num_threads,spin,time,inc_per_itr" << std::endl;
        for (int j = 0; j < n; j++) {
            inc = args[j][1];
            count_test(args[j][0], true);
            count_test(args[j][0], false);
        }
    }
    return 0;
}