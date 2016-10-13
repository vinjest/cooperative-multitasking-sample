#ifndef SHEDULER.H

struct thread_t* create_thread(void(*func_ptr), void* stack_ptr);
void* stack_allocate();
void join(struct thread_t* thread_ptr);
void set_priority(struct thread_t* thread_ptr, int priority);
struct mutex_t* create_mutex();
void acquire_mutex(struct mutex_t* mtx);
void release_mutex(struct mutex_t* mtx);
void resume(struct thread_t* thread_ptr);
void pause(struct thread_t* thread_ptr);

struct mutex_t
{
	int owner_id;
	int counter;
};

#endif
