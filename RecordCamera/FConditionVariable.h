#pragma once
#include "FCGlobalConfig.h"
#include "FCLockMutex.h"
namespace EmrysTool
{
	class FConditionVariable
	{
	public:
		// Construct a cv for use with ONLY one user lock.
		explicit FConditionVariable(FCLockMutex* lock);

		~FConditionVariable();

		void Wait();
		void TimedWait(int millSeconds);

		// Broadcast() revives all waiting threads.
		void Broadcast();
		// Signal() revives one waiting thread.
		void Signal();
		class FCEvent {
		public:
			FCEvent();
			~FCEvent();
			void InitListElement();

			bool IsEmpty() const;
			void PushBack(FCEvent* other);
			FCEvent* PopFront();
			FCEvent* PopBack();


			HANDLE handle() const;

			FCEvent* Extract();

			bool IsSingleton() const;

		private:

			bool ValidateAsDistinct(FCEvent* other) const;
			bool ValidateAsItem() const;
			bool ValidateAsList() const;
			bool ValidateLinks() const;

			HANDLE handle_;
			FCEvent* next_;
			FCEvent* prev_;
			CLASS_COPY_AND_ASSIGN(FCEvent);
		};

		enum RunState { SHUTDOWN = 0, RUNNING = 64213 };

		// Internal implementation methods supporting Wait().
		FCEvent* GetEventForWaiting();
		void RecycleEvent(FCEvent* used_event);

		RunState run_state_;

		// Private critical section for access to member data.
		EmrysTool::FCLockMutex internal_lock_;

		// Lock that is acquired before calling Wait().
		EmrysTool::FCLockMutex& user_lock_;

		// Events that threads are blocked on.
		FCEvent waiting_list_;

		// Free list for old events.
		FCEvent recycling_list_;
		int recycling_list_size_;

		// The number of allocated, but not yet deleted events.
		int allocation_counter_;


		CLASS_COPY_AND_ASSIGN(FConditionVariable);
	};
}


