/********************************************************************

	memory.h - memory handling routines

  Niksa Orlic, 2005-03-22

********************************************************************/

struct memory_object_struct
{
	struct memory_object_struct *next;
	struct memory_object_struct *prev;
	char *data;
};

typedef struct memory_object_struct memory_object;

void mem_init();
void mem_close();

void* mem_alloc(size_t);

// j-a-s-d
#ifdef __GNUC__
void* mem_realloc(char*, int);
void mem_free(char*);
#else
void* mem_realloc(void*, int);
void mem_free(void*);
#endif
