//
// Created by Alex on 2/10/2019.
//
#include <iostream>
#include <chrono>
#include <algorithm>
#include <cstring>

// TODO: Contention is still huge.
auto target_num = 100000000;

struct counter_set {
    uint32_t *counters;
    pthread_spinlock_t *spinlocks;
    pthread_mutex_t *mutex;
    uint32_t target_num;
    int size;
};

counter_set *new_counter_set(int size) {
    auto res = (counter_set *) malloc(sizeof(counter_set));
    res->size = size;
    res->target_num = (target_num / size) * size;
    res->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t) * size);
    res->counters = (uint32_t *) malloc(sizeof(uint32_t) * size);
    res->spinlocks = (pthread_spinlock_t *) malloc(sizeof(pthread_spinlock_t) * size);
    for (int i = 0; i < size; i++) {
        res->counters[i] = 0;
        pthread_spin_init(&res->spinlocks[i], 0);
        pthread_mutex_init(&res->mutex[i], nullptr);
    }

    return res;
}

void free_counter_set(counter_set *s) {
    free(s->counters);
    free(s->mutex);
//    free(s->spinlocks);
    free(s);
}

void *spin_count(void *s_ptr) {
    auto s = (counter_set *) s_ptr;
    auto num = s->target_num / s->size;
    while (num > 0) {
        auto i = rand() % s->size;
        pthread_spin_lock(&s->spinlocks[i]);
        s->counters[i]++;
        pthread_spin_unlock(&s->spinlocks[i]);
        num--;
    }
    pthread_exit(nullptr);
}


void *mutex_count(void *s_ptr) {
    auto s = (counter_set *) s_ptr;
    auto num = s->target_num / s->size;
    while (num > 0) {
        auto i = rand() % s->size;
        pthread_mutex_lock(&s->mutex[i]);
        s->counters[i]++;
        pthread_mutex_unlock(&s->mutex[i]);
        num--;
    }
    pthread_exit(nullptr);
}

void count_test(int num_threads, int counter_set_sz, bool spin) {
    pthread_t threads[num_threads];
    counter_set *s = new_counter_set(counter_set_sz);

    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], nullptr, (spin) ? spin_count : mutex_count, (void *) s);
    }
    // Wait for the threads to finish.
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], nullptr);
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    free_counter_set(s);
    // Print columns: num_threads,spin,time,num_counters
    std::cout << num_threads << ","
              << std::boolalpha << spin << ","
              << std::fixed << elapsed_seconds.count() * 1000 << ","
              << s->size << std::endl;

}

// TODO: Run with `./cc_spinlock < ./cc_spinlock.testcase > ./cc_spinlock.csv`.
int main(int argc, char *argv[]) {
    // Parse input.
    int n = 0;
    std::cin >> n;

    // Arguments are the number of threads and number of counters
    int args[n][2];
    for (int j = 0; j < n; j++) {
        int tmp = 0;
        std::cin >> tmp;
        args[j][0] = tmp;
        std::cin >> tmp;
        args[j][1] = tmp;
    }

    if (n > 0) {
        std::cout << "num_threads,spin,time,num_counters" << std::endl;
        for (int j = 0; j < n; j++) {
            auto seed = time(NULL);
            srand(seed);
            count_test(args[j][0], args[j][1], true);
            srand(seed);
            count_test(args[j][0], args[j][1], false);
        }
    }
    return 0;
}