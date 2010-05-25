/********************************************************************
	
	main.c - compiler entry-point

  Niksa Orlic, 2004-04-19

********************************************************************/

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FILE *yyin;

extern string* build_configuration_define; /* used in lex scanner by the preprocessor; contains the name of the build configuration */

extern string *string_constant;
char *output_path;
char *source_file_name;

extern short int error_count;
extern short int warning_count;

extern int usesForms; /* this is set to 1 if F.class must be include in the generated JAR file */
extern int usesSupport;
extern int usesFloat;
extern int usesRecordStore;
extern int usesHttp;
extern int usesRegisteredFeature;
extern int usesPlayer;

int main(int argc, char **argv)
{

	FILE *fp;
	char source_file[1024];
	char output_directory[1024];

	mem_init();

	if (argc >= 2)
	{
		int len, pos;
		fp = fopen(argv[1], "r");
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

		yyin = fopen(source_file, "r");
		if (yyin == NULL)
			die(MSG_005);

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
		
		if ((yyin != NULL)
			&& (output_path != NULL))
		{
			// TODO:: promijeni ovo
			build_configuration_define = string_from_cstr("DeFaUlt");

			lowercase(string_get_cstr(build_configuration_define));
			
			fprintf(stderr, "Compiling \'%s\'...\n", source_file_name);
			strcpy(output_path, output_directory);
			parser_start();

			fclose(yyin);
			string_destroy(build_configuration_define);
			mem_free(output_path);
			mem_free(source_file_name);
		}
	}

	if (string_constant != NULL)
		string_destroy(string_constant);

	if ((error_count == 0) && (usesPlayer))
			fprintf(stderr, "The generated MIDlet will run only on MIDP-2.0 compatible devices.\n");

	fprintf(stderr, "Done - %d error(s), %d warning(s)\n", error_count, warning_count);
	

	if (error_count == 0)
	{
		if (usesForms)
			fprintf(stderr, "<<FS.class\n");

		if (usesSupport)
			fprintf(stderr, "<<S.class\n");

		if (usesFloat == 1)
			fprintf(stderr, "<<Float1.class\n");

		if (usesFloat == 2)
			fprintf(stderr, "<<Float2.class\n");

		if (usesRecordStore == 1)
			fprintf(stderr, "<<RS.class\n");

		if (usesHttp == 1)
			fprintf(stderr, "<<H.class\n");

		if (usesPlayer == 1)
			fprintf(stderr, "<<P.class\n");

		if (usesRegisteredFeature)
			fprintf(stderr, "<<REGISTERED\n");
	}


	mem_close();

	return 0;
}