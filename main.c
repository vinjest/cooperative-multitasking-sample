#include <stdio.h>
#include "scheduler.h"

void func_first();
void func_second();

int main()
{
    sleep(3000);
    struct thread_t* first = create_thread(&func_first);
    struct thread_t* second = create_thread(&func_second);

    yield();

    join(first);
    join(second);

    sleep(3000);
    puts("Main thread end!");
    return 0;
}

void func_first()
{
    sleep(5000);
    puts("First thread start!");
    yield();
    puts("First thread end!");

}

void func_second()
{
    sleep(10000);
    puts("Second thread start!");
    yield();
    puts("Second thread end!");
}
