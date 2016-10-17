#include "stdafx.h"
#include "sheduler.h"
#include "enums.h"
#include "list.h"

void func_first();
void func_second();
void func_third();

struct thread_t* mutex;

int main()
{
	puts("Main thread start!");
	mutex = create_mutex();

	struct thread_t* first = create_thread(&func_first, stack_allocate());
	struct thread_t* third = create_thread(&func_third, stack_allocate());

	join(first);
	join(third);

	puts("Main thread end!");
	getchar();
	return 0;
}

void func_first()
{	
	puts("First thread start!");
	struct thread_t* second = create_thread(&func_second, stack_allocate());
	join(second);
	next();
	puts("First thread end!");
}

void func_second()
{
	acquire_mutex(mutex);

	puts("Second thread start!");
	next();
	puts("Second thread end!");

	release_mutex(mutex);
}

void func_third()
{
	acquire_mutex(mutex);

	puts("Third thread start!");
	next();
	puts("Third thread end!");

	release_mutex(mutex);
}
