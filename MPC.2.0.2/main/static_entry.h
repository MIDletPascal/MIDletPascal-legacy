/*
	Library export defines
*/

void (*compile_terminate)();
void (*compile_write2)(char*, char*);
void (*compile_write3)(char*, char*, char*);
void (*compile_progress)(int);

struct compile_result_struct
{
	short int error_count;
	short int warning_count;

	int usesForms; /* this is set to 1 if F.class must be include in the generated JAR file */
	int usesSupport;
	int usesFloat;
	int usesRecordStore;
	int usesHttp;
	int usesRegisteredFeature;
	int usesPlayer;
	int usesSMS;
	char *userLibraries;
	char *units;
};

typedef struct compile_result_struct compile_result;

#ifndef IDE_INCLUDE
extern compile_result __cdecl compiler_run(char* buildFile,
				char *library_directory_arg,
				char *build_configuration,
				  void (*compile_terminate_arg)(),
				  void (*compile_write2_arg)(char*, char*),
				  void (*compile_write3_arg)(char*, char*, char*),
				  void (*compile_progress)(int),
				  int canvasType_arg,
				  int mathType_arg,
				  int next_record_ID_arg,
				  int detect_units_only_arg,
				  char **units);
#endif

/* canvas types */
#define NORMAL		0
#define FULL_MIDP20 1
#define FULL_NOKIA	2
