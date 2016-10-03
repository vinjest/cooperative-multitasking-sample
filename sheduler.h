#ifndef SHEDULER.H

void exec(void(*p_func_first)(), ...);
void next();
void sleep();
void wakeup(int* func_ptr);
void enter_critical_section();
void leave_critical_section();

#endif