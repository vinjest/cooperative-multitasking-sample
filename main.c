#include <stdio.h>
#include "scheduler.h"

void func_first();
void func_second();

struct thread_t* first = NULL;
struct thread_t* second = NULL;
struct mutex_t* mutex = NULL;

int main()
{
    puts("Main thread start!");
    mutex_init(&mutex);

    first = create_thread(&func_first);
    second = create_thread(&func_second);

    yield();

    join(first);
    join(second);

    puts("Main thread end!");
    mutex_remove(&mutex);
    return 0;
}

void func_first()
{
    acquire_mutex(&mutex);

    puts("First thread start!");
    yield();
    puts("First thread end!");

    release_mutex(&mutex);
}

void func_second()
{
    acquire_mutex(&mutex);

    puts("Second thread start!");
    yield();
    puts("Second thread end!");

    release_mutex(&mutex);
}
