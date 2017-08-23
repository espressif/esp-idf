#include <iostream>
#include <thread>
#include <mutex>
#include "unity.h"
#include "rom/ets_sys.h"

std::shared_ptr<int> global_sp;
std::mutex mtx;

void thread_main() {
    int i = 0;
    //std::cout << "thread_main!" << std::endl;
    ets_printf("thread_main\n");

    while (1) {
        // std::cout << "thread_main " << i << std::endl;
        ets_printf("thread_main %d\n", i);
        i++;
    }
    // auto local_sp = global_sp;  // OK, copy constructor's parameter is reference-to-const

    // int i = *global_sp;         // OK, operator* is const
    // int j = *local_sp;          // OK, does not operate on global_sp

    // *global_sp = 2;          // NOT OK, modifies int visible to other threads
    // *local_sp = 2;           // NOT OK, modifies int visible to other threads

    // mtx.lock();
    // global_sp.reset();       // NOT OK, reset is non-const
    // mtx.unlock();
    //local_sp.reset();           // OK, does not operate on global_sp
}

//extern "C"
TEST_CASE("pthread CXX test 1", "[pthread]")
{
    std::cout << "Hello world!" << std::endl;

    global_sp.reset(new int(1));
    std::thread t1(thread_main);
    // std::thread t2(thread_main);
    t1.join();
    // t2.join();
}
