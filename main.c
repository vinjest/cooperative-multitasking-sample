#include "stdafx.h"
#include "sheduler.h"
#include "enums.h"

#define separator -1

void func_first();
void func_second();
void func_third();

int main()
{
	exec(&func_first, &func_second, &func_third, separator, high, medium, low, separator);
	getchar();
	return 0;
}

void func_first()
{
	sleep();
	puts("First task start");
	next();
	enter_critical_section();
	puts("First task end!");
	leave_critical_section();
}

void func_second()
{
	enter_critical_section();
	puts("Second task start!");
	wakeup(&func_first);
	next();
	puts("Second task end!");
	leave_critical_section();
}

void func_third()
{
	puts("Third task start!");
	sleep();
	puts("Third task end!");
}