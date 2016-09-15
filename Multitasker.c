#include <stdio.h>
#include <setjmp.h>
#include "list.h"

void func_first(int func_id);
void func_second(int func_id);
void func_third(int func_id);
void yield(int func_id);
void execute(WorkerTask_1, WorkerTask_2, WorkerTask_3);

struct Task* task_list;

#define WorkerTask_1 void(*p_func_first)(int func_id)
#define WorkerTask_2 void(*p_func_second)(int func_id)
#define WorkerTask_3 void(*p_func_third)(int func_id)
WorkerTask_1 = &func_first;
WorkerTask_2 = &func_second;
WorkerTask_3 = &func_third;

#define max_task_count 3
#define next if (!setjmp(func_contexts[func_id])) yield(func_id);

static jmp_buf main_context;
static jmp_buf func_contexts[max_task_count][16];

int* buf_boxing(int buf[16]);

int main()
{
	execute(p_func_first, p_func_second, p_func_third);
	getchar();
	clear(task_list);
	return 0;
}

void func_first(int func_id)
{
	puts("First task start");

	next

	puts("First task continue");
}

void func_second(int func_id)
{
	puts("Second task start");

	next

	puts("Second task continue");

}

void func_third(int func_id)
{
	puts("Third task start");

	next

	puts("Third task continue");
}

void execute(WorkerTask_1, WorkerTask_2, WorkerTask_3)
{
	char space[1000]; /* Reserve enough space for main to run */
	space[999] = 1;
	if (!setjmp(main_context))
	{
		p_func_first(0);
		longjmp(main_context,1);
	}
	if (!setjmp(main_context))
	{
		p_func_second(1);
		longjmp(main_context, 1);
	}
	if (!setjmp(main_context))
	{
		p_func_third(2);
		longjmp(main_context, 1);
	}
	
	int task_num = 0;
	for (task_num = 0; task_num < max_task_count; task_num++)
	{
		if (!setjmp(main_context))
			yield(777);
	}
}

void yield(int func_id)
{
	if (func_id == 777)
	{
		int* buf_boxed = pop_front(&task_list);
		int buf_unboxed[16];
		int i = 0;
		for (i = 0; i < 16; i++)
			buf_unboxed[i] = buf_boxed[i];
		longjmp(buf_unboxed, 1);
	}

	if (size(&task_list) <= max_task_count)
	{
		push_back(&task_list, buf_boxing(func_contexts[func_id]));
		longjmp(main_context, 1);
	}
}

int* buf_boxing(int buf[16])
{
	int* arr = (int*)malloc(sizeof(jmp_buf));
	int i = 0;
	for (i = 0; i < 16; i++)
	{
		arr[i] = buf[i];
	}
	return arr;
}

