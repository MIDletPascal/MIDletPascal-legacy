/*
	static_entry.c - the file with library exports
*/

#include "../util/strings.h"
#include "../util/error.h"
#include "../util/message.h"
#include "../structures/type_list.h"
#include "../structures/string_list.h"
#include "../structures/type.h"
#include "../structures/identifier.h"
#include "../structures/name_table.h"
#include "../classgen/bytecode.h"
#include "../structures/block.h"

#include "../parser/parser.h"

#include "../util/memory.h"

//#include "static_entry.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* buildFile;
////////
char *library_directory;
char *build_configuration;
int canvasType;
int mathType;
int next_record_ID;
int detetct_units_only;
char **units;


void compile_terminate(){
	fprintf(stderr, "compile_terminate\n");
	exit(1);
};

void compile_write3(char* x, char* y, char* z){
	//fprintf(stderr, x,y,z);
	fprintf(stdout,x,y);
};

void compile_progress(int x){
	//fprintf(stderr, "%i,",x);
};

//////////

extern FILE *yyin;

extern string* build_configuration_define; /* used in lex scanner by the preprocessor; contains the name of the build configuration */

extern string *string_constant;
char *output_path;
char *source_file_name;
char *library_directory;
char *user_libraries = NULL;
char **units;

extern short int error_count;
extern short int warning_count;

extern int usesForms; /* this is set to 1 if F.class must be include in the generated JAR file */
extern int usesSupport;
extern int usesFloat;
extern int usesRecordStore;
extern int usesHttp;
extern int usesRegisteredFeature;
extern int usesPlayer;
extern int usesSMS;

int canvasType;
int mathType;

int fileSize;

extern int constant_pool_size;
extern long int last_error_linenum;
extern int inside_loop;
extern int usesForms;
extern int usesSupport;
extern int usesFloat;
extern int usesRecordStore;
extern int usesHttp; 
extern int usesPlayer;

extern int usesRegisteredFeature;

extern int linenum;
extern string *string_constant;
extern int warning_101_count;

extern int next_record_ID;

int detect_units_only; //***

extern void yyrestart(FILE *new_file);

void initialize_compiler()
{
	constant_pool_size = 0;
	error_count = 0;
	warning_count = 0;
	last_error_linenum = -1;
	inside_loop = 0;

	usesForms = 0;
	usesSupport = 0;
	usesFloat = 0;
	usesRecordStore = 0;
	usesHttp = 0; 
	usesPlayer = 0;
	usesRegisteredFeature = 0;
	usesSMS = 0;
	user_libraries = NULL;

	linenum = 1;
	string_constant = NULL;

	warning_101_count = 0;
}


int main(int argc, char **argv)
{
/*extern compile_result __cdecl compiler_run(char* buildFile,
				  char *library_directory_arg,
				  char *build_configuration,
				  void (*compile_terminate_arg)(),
				  void (*compile_write2_arg)(char*, char*),
				  void (*compile_write3_arg)(char*, char*, char*),
				  void (*compile_progress_arg)(int),
				  int canvasType_arg,
				  int mathType_arg,
				  int next_record_ID_arg,
				  int detetct_units_only_arg,
				  char **units_arg)
{
*/
	FILE *fp;	
	
	char source_file[1024];
	char output_directory[1024];
	char final_message[1024];
	
	int len, pos;
	int testVar;

	/*compile_result returnStructure;
	compile_terminate = compile_terminate_arg;
	compile_write2 = compile_write2_arg;
	compile_write3 = compile_write3_arg;
	compile_progress = compile_progress_arg;
	*/
    if (argc >= 2)
	{
		/*
		units = units_arg;
		detect_units_only = detetct_units_only_arg;
		canvasType = canvasType_arg;
		mathType = mathType_arg;
		next_record_ID = next_record_ID_arg;
		library_directory = library_directory_arg;
		*/
		mem_init();
		initialize_compiler();
		units=malloc(8);
		*units=malloc(8);
		*units=NULL;
        
		fp = fopen(argv[1]/*buildFile*/, "r");
		if (fp == NULL)
			die(MSG_005);

		fgets(source_file, 1024, fp);
		fgets(output_directory, 1024, fp);

		pos = strlen(source_file) - 1;
		while ((source_file[pos] == '\r')
			|| (source_file[pos] == '\n'))
			pos --;
		source_file[pos + 1] = '\0';

		pos = strlen(output_directory) - 1;
		while ((output_directory[pos] == '\r')
			|| (output_directory[pos] == '\n'))
			pos --;
		output_directory[pos + 1] = '\0';

		fclose(fp);

		fp = fopen(source_file, "r");
	
		if (fp == NULL)
			die(MSG_005);

		fseek(fp, 0, SEEK_END);
		fileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
	
		yyrestart(fp);

		len = strlen(source_file);
		source_file_name = (char*) mem_alloc(len + 1);
		if (source_file_name == NULL)
			die(MSG_001);

		pos = len - 1;
		while ((pos >= 0) && (source_file[pos] != '\\'))
			pos --;
	
		if (source_file[pos] != '\\')
			die(MSG_021);
		pos ++;

		strcpy(source_file_name, source_file + pos);

		len = strlen(output_directory);
		output_path = (char*) mem_alloc(len + 1);

		////////
		library_directory=output_directory;
	
		if ((yyin != NULL)
			&& (output_path != NULL))
		{
			/*
			if (build_configuration != NULL)
				build_configuration_define = string_from_cstr(build_configuration);
			else
				build_configuration_define = string_from_cstr("");
			*/
			build_configuration_define = string_from_cstr("");
			
	
			lowercase(string_get_cstr(build_configuration_define));
           
			compile_write3("Compiling \'%s\'...\n", source_file_name, "");
			strcpy(output_path, output_directory);
			parser_start();

			fclose(yyin);
			mem_free(output_path);
			mem_free(source_file_name);
		}
	}
	if (string_constant != NULL)
		string_destroy(string_constant);
	
	fprintf(stderr, "Done - %d error(s), %d warning(s)\n", error_count, warning_count);
	if (error_count == 0)
	{
		if (usesForms)
			compile_write3("<<FS.class\n","","");
		if (usesSupport)
			compile_write3("<<S.class\n","","");
		if (usesFloat == 1)
			compile_write3("<<Float1.class\n","","");
		if (usesFloat == 2)
			compile_write3("<<Float2.class\n","","");
		if (usesRecordStore == 1)
			compile_write3("<<RS.class\n","","");
		if (usesHttp == 1)
			compile_write3("<<H.class\n","","");
		if (usesPlayer == 1)
			compile_write3("<<P.class\n","","");
		if (usesSMS == 1)
			compile_write3("<<SM.class\n","","");
		if (usesRegisteredFeature)
			compile_write3("<<REGISTERED\n","","");
	}
	mem_close();
    return 0;
}