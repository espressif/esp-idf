#include <iostream>
#include <thread>
#include <mutex>
#include "unity.h"

std::shared_ptr<int> global_sp;
std::mutex mtx;

static void thread_main() {
    int i = 0;
    std::cout << "thread_main CXX " << std::hex <<  std::this_thread::get_id() << std::endl;

    while (i < 10) {
	    // mux test
	    mtx.lock();
		// yield test
		std::this_thread::yield();
	    (*global_sp)++;
	    mtx.unlock();
        std::cout << "thread " << std::hex << std::this_thread::get_id() << ": " << i++ << " val= " << *global_sp << std::endl;
	    // sleep_for test
        std::chrono::milliseconds dur(300);
		std::this_thread::sleep_for(dur);
	    // sleep_until test
		using std::chrono::system_clock;
  		std::time_t tt = system_clock::to_time_t(system_clock::now());	    
	    struct std::tm *ptm = std::localtime(&tt);
		ptm->tm_sec = 1;
		std::this_thread::sleep_until(system_clock::from_time_t (mktime(ptm)));
    }
}

TEST_CASE("pthread CXX test 1", "[pthread]")
{
    std::cout << "TEST START!" << std::endl;

    global_sp.reset(new int(1));
    std::thread t1(thread_main);
    std::thread t2(thread_main);
    if (t1.joinable()) {
    	std::cout << "Join thread " << std::hex << t1.get_id() << std::endl;
    	t1.join();
    }
    if (t2.joinable()) {
    	std::cout << "Join thread " << std::hex << t2.get_id() << std::endl;
    	t2.join();
    }
    std::cout << "TEST END!" << std::endl;
}
