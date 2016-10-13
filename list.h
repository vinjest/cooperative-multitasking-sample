#ifndef LIST_H
#define LIST_H

int push_back(struct thread_t** head, int* func_ptr, int current_id, enum priority current_priority, void* stack_base_ptr);
struct thread_t* pop_front(struct thread_t** head);
struct thread_t* pop_back(struct thread_t** head);
int clear(struct thread_t** head);
int size(struct thread_t** head);
struct thread_t* value_at(struct thread_t** head, int position);
int remove_by_id(struct thread_t** head, int current_id);
struct thread_t* get_thread_t_with_max_priority(struct thread_t** head);
struct thread_t* get_by_id(struct thread_t** head, int current_id);

struct thread_t
{
	int id;
	int* saved_context;
	void* stack_base;
	struct thread_t* next;
	int* func_ptr;
	enum state state;
	enum priority priority;
	struct thread_t* subscriber;
};

#endif
