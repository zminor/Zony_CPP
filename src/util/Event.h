#ifndef __EVENT_H__
#define __EVENT_H__

#include <mutex>
#include <condition_variable>
#include <atomic>

namespace Utils
{
 class Event
 {
 	public:
		Event(const bool signaled = false,const bool manualy = false) noexcept;
		~Event() noexcept = default;
		
	private:
		std::mutex mtx;							//Mutex
		std::condition_variable cv;	//condition
		std::atomic<bool> signaled;  //Notified
		bool manually;
	
	public:
		void wait();
		bool  wait_for(const std::chrono::milliseconds &ms);
		bool wait_until(const std::chrono::high_resolution_clock::time_point &tp);
		
		void notify() noexcept;
		void notify( const size_t threadCount) noexcept;

		void reset() noexcept;
	
		bool notified() const noexcept;
 };

}


#endif
