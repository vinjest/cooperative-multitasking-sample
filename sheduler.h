#ifndef SHEDULER_H
#define SHEDULER_H

void yield();
void thread_start(void(*func_ptr)());
void create_thread(void(*func_ptr)());

#endif
