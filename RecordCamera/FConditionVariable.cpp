#include "stdafx.h"
#include "FConditionVariable.h"



#include <stack>

namespace EmrysTool {

	FConditionVariable::FConditionVariable(FCLockMutex* user_lock):user_lock_(*user_lock),
		run_state_(RUNNING),
		allocation_counter_(0),
		recycling_list_size_(0) {
	}

	FConditionVariable::~FConditionVariable() {
		FCAutoUnlock auto_lock(internal_lock_);
		run_state_ = SHUTDOWN;  
		if (recycling_list_size_ != allocation_counter_) {  
			FCAutoUnlock auto_unlock(internal_lock_);
			Broadcast();  
			Sleep(10);  
		}  
	}

	void FConditionVariable::Wait() {
		TimedWait(INFINITE);
	}

	void FConditionVariable::TimedWait(int millSeconds) {
		FCEvent* waiting_event;
		HANDLE handle;
		{
			FCAutoUnlock auto_lock(internal_lock_);
			if (RUNNING != run_state_) return; 
			waiting_event = GetEventForWaiting();
			handle = waiting_event->handle();
			
		}  

		{
			FCAutoUnlock unlock(user_lock_);  // Release caller's lock
			WaitForSingleObject(handle, static_cast<DWORD>(millSeconds));
			FCAutoUnlock auto_lock(internal_lock_);
			RecycleEvent(waiting_event);
			
		}  
	}

	// Broadcast() is guaranteed to signal all threads that were waiting (i.e., had
	// a cv_event internally allocated for them) before Broadcast() was called.
	void FConditionVariable::Broadcast() {
		std::stack<HANDLE> handles;  // See FAQ-question-10.
		{
			FCAutoUnlock auto_lock(internal_lock_);
			if (waiting_list_.IsEmpty())
				return;
			while (!waiting_list_.IsEmpty())
					handles.push(waiting_list_.PopBack()->handle());
		}  
		while (!handles.empty()) {
			SetEvent(handles.top());
			handles.pop();
		}
	}
	void FConditionVariable::Signal() {
		HANDLE handle;
		{
			FCAutoUnlock auto_lock(internal_lock_);
			if (waiting_list_.IsEmpty())
				return;  
			handle = waiting_list_.PopBack()->handle();  // LIFO.
		}  
		SetEvent(handle);
	}

	FConditionVariable::FCEvent* FConditionVariable::GetEventForWaiting() {
		// We hold internal_lock, courtesy of Wait().
		FCEvent* cv_event;
		if (0 == recycling_list_size_) {
			//DCHECK(recycling_list_.IsEmpty());
			cv_event = new FCEvent();
			cv_event->InitListElement();
			allocation_counter_++;
			//DCHECK(cv_event->handle());
		} else {
			cv_event = recycling_list_.PopFront();
			recycling_list_size_--;
		}
		waiting_list_.PushBack(cv_event);
		return cv_event;
	}

	
	void FConditionVariable::RecycleEvent(FCEvent* used_event) {

		used_event->Extract();  // Possibly redundant
		recycling_list_.PushBack(used_event);
		recycling_list_size_++;
	}
	

	FConditionVariable::FCEvent::FCEvent() : handle_(0) {
		next_ = prev_ = this;  // Self referencing circular.
	}

	FConditionVariable::FCEvent::~FCEvent() {
		if (0 == handle_) {
			// This is the list holder
			while (!IsEmpty()) {
				FCEvent* cv_event = PopFront();
				//DCHECK(cv_event->ValidateAsItem());
				delete cv_event;
			}
		}
		if (0 != handle_) {
			int ret_val = CloseHandle(handle_);
		
		}
	}
	void FConditionVariable::FCEvent::InitListElement() {
		handle_ = CreateEvent(NULL, false, false, NULL);
	}

	bool FConditionVariable::FCEvent::IsEmpty() const {
		return IsSingleton();
	}

	void FConditionVariable::FCEvent::PushBack(FCEvent* other) {

		other->prev_ = prev_;
		other->next_ = this;
		prev_->next_ = other;
		prev_ = other;
	}

	FConditionVariable::FCEvent* FConditionVariable::FCEvent::PopFront() {
		return next_->Extract();
	}

	FConditionVariable::FCEvent* FConditionVariable::FCEvent::PopBack() {
		//DCHECK(ValidateAsList());
		//DCHECK(!IsSingleton());
		return prev_->Extract();
	}

	HANDLE FConditionVariable::FCEvent::handle() const {
		return handle_;
	}

	// Pull an element from a list (if it's in one).
	FConditionVariable::FCEvent* FConditionVariable::FCEvent::Extract() {
		if (!IsSingleton()) {
			next_->prev_ = prev_;
			prev_->next_ = next_;
			prev_ = next_ = this;
		}
		return this;
	}

	bool FConditionVariable::FCEvent::IsSingleton() const {
		return next_ == this;
	}

	// Provide pre/post conditions to validate correct manipulations.
	bool FConditionVariable::FCEvent::ValidateAsDistinct(FCEvent* other) const {
		return ValidateLinks() && other->ValidateLinks() && (this != other);
	}

	bool FConditionVariable::FCEvent::ValidateAsItem() const {
		return (0 != handle_) && ValidateLinks();
	}

	bool FConditionVariable::FCEvent::ValidateAsList() const {
		return (0 == handle_) && ValidateLinks();
	}

	bool FConditionVariable::FCEvent::ValidateLinks() const {

		return (next_->prev_ == this) && (prev_->next_ == this);
	}

} 

