#include <stdio.h>
#include "sheduler.h"

void func_first();
void func_second();

int main()
{
    create_thread(&func_first);
    create_thread(&func_second);

    yield();

    return 0;
}

void func_first()
{
    puts("First thread start!");
    yield();
    puts("First thread end!");
}

void func_second()
{
    puts("Second thread start!");
    yield();
    sleep(1000);
    puts("Second thread end!");
}
