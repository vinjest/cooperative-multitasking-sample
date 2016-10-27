#ifndef SHEDULER_H
#define SHEDULER_H

void yield();
void thread_start(void(*func_ptr)(), struct thread_t* added_thread_ptr);
void create_thread(void(*func_ptr)());
void sleep(long milliseconds);

#endif
