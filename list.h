#ifndef LIST_H
#define LIST_H

int push_back(struct Task** head, int* func_ptr, enum priority current_priority, int current_id);
struct Task* pop_front(struct Task** head);
struct Task* pop_back(struct Task** head);
int clear(struct Task** head);
int size(struct Task** head);
struct Task* value_at(struct Task** head, int position);
int remove_by_id(struct Task** head, int current_id);
struct Task* get_task_with_max_priority(struct Task** head);

struct Task
{
	int id;
	int* saved_eip;
	int* saved_esp;
	struct Task* next;
	int* func_ptr;
	enum state state;
	enum priority priority;
};

#endif