/********************************************************************

	constant_pool.h - methods for handling constant pool

  Niksa Orlic, 2004-06-11

********************************************************************/

struct constant_pool_struct
{
	unsigned char tag;
	unsigned short param1;
	unsigned short param2;
	char *data;
	int data_len;
};

typedef struct constant_pool_struct constant_pool;

int cp_add_string(char *str);
int cp_add_utf8(char *str);
int cp_add_class(char *class_name);
int cp_add_integer(int data);
int cp_add_long(long data);
int cp_add_float(float data);
int cp_add_double(double data);
int cp_add_fieldref(char *class_name, char *name, char *type);
int cp_add_methodref(char *class_name, char *name, char *type);
int cp_add_interface(char *class_name, char *name, char *type);
int cp_add_nameandtype(char *name, char *type);
constant_pool *cp_create_new_entry();

void write_constant_pool(FILE *fp);
