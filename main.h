typedef enum{
	LOW_PRIORITY,
	MEDIUM_PRIORITY,
	HIGH_PRIORITY,
}Priority_t;

typedef enum{
	THREAD_READY_STATE,
	THREAD_RUNNING_STATE,
	THREAD_BLOCKED_STATE,
}ThreadState_t;

typedef struct{
	char* thread_id;
	uint32_t* thread_sp;
	Priority_t priority;
	ThreadState_t state;
	uint32_t* thread_base_ptr;
	uint32_t ThreadStackSize;
	void (*fptr)(void); 	// fp to thread execution function.
}TCB_t;