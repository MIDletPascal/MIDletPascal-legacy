/*
	MidletPascal-compiler
*/
#include "../util/strings.h"
#include "../util/error.h"
//#include "../util/message.h"
#include "../structures/type_list.h"
#include "../structures/string_list.h"
#include "../structures/type.h"
#include "../structures/identifier.h"
#include "../structures/name_table.h"
#include "../classgen/bytecode.h"
#include "../structures/block.h"
#include "../parser/parser.h"
#include "../util/memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char msgstr[1024];
char *library_directory;
char *build_configuration;
int canvasType;
int mathType;
int next_record_ID;
int detetct_units_only;
char **units;
int goverify;

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
//extern int usesRegisteredFeature;
extern int usesPlayer;
extern int usesSMS;

//int canvasType;
//int mathType;


extern int constant_pool_size;
extern long int last_error_linenum;
extern int inside_loop;

//extern int usesForms;
//extern int usesSupport;
//extern int usesFloat;
//extern int usesRecordStore;
//extern int usesHttp; 
//extern int usesPlayer;
//extern int usesRegisteredFeature;

extern int linenum;
extern string *string_constant;
extern int warning_101_count;

extern int next_record_ID;

int detect_units_only; //***

void compileMSG(){
	fprintf(stdout /*stderr*/, &msgstr);
};


void _compile_progress(int x){
	sprintf(msgstr,"\r%d ",x);	compileMSG();
};


void compile_terminate(){
	mem_close();
	fclose(yyin);
	exit(1);
};


//////////


void initialize_compiler()
{
	constant_pool_size = 0;
	string_constant = NULL;
	warning_101_count = 0;
	error_count = 0;
	warning_count = 0;
	last_error_linenum = -1;
	linenum = 1;
	inside_loop = 0;
	goverify = 1;
	mathType = 1;
	usesFloat = 0;
	usesForms = 0;
	usesSupport = 0;
	usesRecordStore = 0;
	usesHttp = 0; 
	usesPlayer = 0;
	//usesRegisteredFeature = 0;
	usesSMS = 0;
	user_libraries = NULL;
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

	sprintf(msgstr,"MIDletPascal compiler. ver.009 beta\nby Nicsa Orlic (c).  http://sourceforge.net/projects/midletpascal/\n");
	compileMSG();


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
		fp = fopen(argv[1], "r");
		if (fp == NULL) die(5);
		fgets(source_file, 1024, fp);
		fgets(output_directory, 1024, fp);
		fclose(fp);

		pos = strlen(source_file) - 1;
		while ((source_file[pos] == '\r') || (source_file[pos] == '\n')) pos --;
		source_file[pos + 1] = '\0';
		source_file_name = (char*) mem_alloc(pos + 1);
		if (source_file_name == NULL)  die(1);
		//pos = len - 1;
		while ((pos >= 0) && (source_file[pos] != '\\'))  pos --;
		if (source_file[pos] != '\\')  die(21);
		//pos ++;
		strcpy(source_file_name, source_file + pos + 1);
		////////////////////
		pos = strlen(output_directory) - 1;
		while ((output_directory[pos] == '\r')|| (output_directory[pos] == '\n')) pos --;
		output_directory[pos + 1] = '\0';
		//len = strlen(output_directory);	output_path = (char*) mem_alloc(len + 1);
		output_path = output_directory;
		library_directory=output_directory;
		yyrestart(source_file);
		if ((yyin != NULL) && (output_path != NULL))
		{
			/*
			if (build_configuration != NULL)
				build_configuration_define = string_from_cstr(build_configuration);
			else
				build_configuration_define = string_from_cstr("");
			*/
			build_configuration_define = string_from_cstr("");
			lowercase(string_get_cstr(build_configuration_define));

			sprintf(msgstr,"Compiling \'%s\'...\n", source_file_name);
			compileMSG();
			strcpy(output_path, output_directory);
			parser_start();

			fclose(yyin);
			mem_free(output_path);
			mem_free(source_file_name);
		}
	}
	if (string_constant != NULL)
		string_destroy(string_constant);

	sprintf(msgstr,"Done - %d error(s), %d warning(s)\n", error_count, warning_count);
	compileMSG();
	if (error_count == 0){
		if (usesForms){
			sprintf(msgstr,"<<FS.class\n");
			compileMSG();
		}
		if (usesSupport){
			sprintf(msgstr,"<<S.class\n");
			compileMSG();
		}
		if (usesFloat == 1){
			if (mathType==1) sprintf(msgstr,"<<Float1.class\n");
				else sprintf(msgstr,"<<Float2.class\n");
			compileMSG();
		}
		if (usesRecordStore == 1){
			sprintf(msgstr,"<<RS.class\n");
			compileMSG();
		}
		if (usesHttp == 1){
			sprintf(msgstr,"<<H.class\n");
			compileMSG();
		}
		if (usesPlayer == 1){
			sprintf(msgstr,"<<P.class\n");
			compileMSG();
		}
		if (usesSMS == 1){
			sprintf(msgstr,"<<SM.class\n");
			compileMSG();
		}
/*		if (usesRegisteredFeature){
			sprintf(msgstr,"<<REGISTERED\n");
			compileMSG();
		}
*/
	}
	mem_close();
    return 0;
}