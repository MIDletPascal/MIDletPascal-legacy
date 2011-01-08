/*

        MidletPascal-compiler 3.3.IDE (mp3CC.exe)
        http://sourceforge.net/projects/midletpascal

        MPC.2.0.2:
            Niksa Orlic

        MPC.3.0.003/009:
            Artem (ironwoodcutter@bk.ru)
                new lexer,
                bytecode inline,
                shl & shr operators,
                smart string concatenation,
                new max array size (up to 32767),
                inclusion of {$R+/-} to enable/disable real numbers support,
                and {$V+/-} to enable/disable internal bytecode preverification

        MPC.3.0.IDE:
            Javier Santo Domingo (j-a-s-d@users.sourceforge.net)
                ported to GNUCC (with coditional defines),
                pascal-like errors messages and warnings,
                new command-line parsing (C way),
                disabled $R and $V directives (confusing overlapped functionality with the IDE),
                and several other adjustments (wow64 WM_COPYDATA workaround, etc)
                and bugfixes (real numbers parsing, shl & shr swapped operators, etc)

        MPC.3.1.IDE:
            Javier Santo Domingo (j-a-s-d@users.sourceforge.net)
                infinite-loop support (repeat/forever),
                and bugfixes (complex-type bidimensional array initialization index out-of-bound, etc)

        MPC.3.2.IDE:
            Javier Santo Domingo (j-a-s-d@users.sourceforge.net)
                exit keyword support,
                C-style multiline comments support

        MPC.3.3.IDE:
            Javier Santo Domingo (j-a-s-d@users.sourceforge.net)
                result keyword support,
                C-style shift operators support (<< and >>),
                bugfixes (const assignment crash, etc)

*/

// j-a-s-d: workaround for Wow64 using WM_COPYDATA
#define WOW64_WORKAROUND

#include "../util/strings.h"
#include "../util/error.h"
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
#include <getopt.h>

#ifdef WOW64_WORKAROUND
    #include <windows.h>

    int ParentNotifyHandle = INVALID_HANDLE_VALUE;
    COPYDATASTRUCT COMPILERMESSAGE;
#endif

char msgstr[1024];
int canvasType;
int mathType;
int next_record_ID;
int detect_units_only;
int goverify;

extern FILE *yyin;
extern string *string_constant;
char *output_path;
char *source_file_name;
char *library_directory;
char *user_libraries = NULL;
char **units;

extern short int error_count;
extern short int warning_count;

extern int usesForms;
extern int usesSupport;
extern int usesFloat;
extern int usesRecordStore;
extern int usesHttp;
extern int usesPlayer;
extern int usesSMS;

extern int constant_pool_size;
extern long int last_error_linenum;
extern int inside_loop;

extern int linenum;
extern string *string_constant;
extern int warning_101_count;

extern int next_record_ID;

void compileMSG()
{
#ifdef WOW64_WORKAROUND
    if (ParentNotifyHandle != INVALID_HANDLE_VALUE) {
        COMPILERMESSAGE.dwData = 1;
        COMPILERMESSAGE.cbData = sizeof( msgstr );
        COMPILERMESSAGE.lpData = &msgstr;
        SendMessage(ParentNotifyHandle, WM_COPYDATA,
            (WPARAM) 0, (LPARAM)(LPVOID) &COMPILERMESSAGE);
        return;
    }
#endif
    printf(msgstr);
    fflush(stdout);
}

void requires(int reqkind, char* req)
{
  switch (reqkind)
  {
    case 0: // unit
        sprintf(msgstr,"^0%s\n", req);
        break;
    case 1: // lib
        sprintf(msgstr,"^1%s\n", req);
        break;
    case 2: // stub
        sprintf(msgstr,"^2%s\n", req);
        break;
    case 3: // record
        sprintf(msgstr,"^3%s\n", req);
        break;
  }
  compileMSG();
}

int last_progress;

void compile_progress(int x)
{
    if (x != last_progress)
    {
        last_progress = x;
        sprintf(msgstr,"@%d\n", last_progress);
        compileMSG();
    }
}

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
	usesFloat = 0;
	usesForms = 0;
	usesSupport = 0;
	usesRecordStore = 0;
	usesHttp = 0;
	usesPlayer = 0;
	usesSMS = 0;
	user_libraries = NULL;
}

void ShowHelp()
{
    sprintf(msgstr,
    "-------------------------------------------------------------------------\n"
    "MIDletPascal 3.3 Compiler\n"
    "Version: MPC.3.3.IDE\n"
    "Authors: Niksa Orlic (1.x-2.0), Artem (3.0), Javier Santo Domingo (3.x)\n"
    "-------------------------------------------------------------------------\n"
    "usage: mp3cc\n"
    "\t-s\"<source_filename>\"\n"
    "\t-o\"<output_path>\"\n"
    "\t-l\"<library_path>\"\n"
    "\t-m<math_type>\n"
    "\t-c<canvas_type>\n"
    "\t-r<next_record_ID>\n"
    "\t[-d] // detect units only\n"
#ifdef WOW64_WORKAROUND
    "\t[-n]<parent_notify_handle> // outputs using WMCOPYDATA\n"
#endif
    "\n"
    );
    compileMSG();
    exit(2);
}

void PrintFinalMessage ()
{
    if (usesForms) {
        requires(2,"FS.class");
    }
    if (usesSupport) {
        requires(2,"S.class");
    }
    if (usesFloat == 1) {
        if (mathType == 2) {
            requires(2,"Real.class");
            requires(2,"Real$NumberFormat.class");
        } else {
            requires(2,"F.class");
        }
    }
    if (usesRecordStore == 1) {
        requires(2,"RS.class");
    }
    if (usesHttp == 1) {
        requires(2,"H.class");
    }
    if (usesPlayer == 1) {
        requires(2,"P.class");
    }
    if (usesSMS == 1) {
        requires(2,"SM.class");
    }
    sprintf(msgstr,"Done - %d error(s), %d warning(s)\n", error_count, warning_count);
}

int main(int argc, char **argv)
{
    error_count = -1;
    char *source_file = NULL;
    char *output_directory = NULL;

	int len, pos;

	if (argc >= 2)
	{
        // -- parse command line arguments
        detect_units_only = 0;
        mathType = 1;
        canvasType = NORMAL;

        int c;
        while ((c = getopt(argc, argv, "?:s:o:l:m:n:c:r:d")) != -1) {
            switch(c) {
                case 's':
                    source_file = optarg;
                    break;
                case 'o':
                    output_directory = optarg;
                    break;
                case 'l':
                    library_directory = optarg;
                    break;
                case 'm':
                    if (strcmp("2", optarg) == 0) {
                      mathType = 2; // REAL
                    }
                    break;
                case 'n':
#ifdef WOW64_WORKAROUND
                    ParentNotifyHandle = atoi(optarg);
#endif
                    break;
                case 'r':
                    next_record_ID = atoi(optarg);
                    break;
                case 'c':
                    if (strcmp("2", optarg) == 0) {
                      canvasType = FULL_NOKIA; // FULL_NOKIA
                    } else if (strcmp("1", optarg) == 0) {
                      canvasType = FULL_MIDP20; // FULL_MIDP20
                    }
                    break;
                case 'd':
                    detect_units_only++;
                    break;
                case '?':
                default:
                    ShowHelp();
            }
        }

        if (library_directory == NULL) {
            sprintf(msgstr,"mp3cc: no library directory\n");
            compileMSG();
            ShowHelp();
        }
        if (output_directory == NULL) {
            sprintf(msgstr,"mp3cc: no output directory\n");
            compileMSG();
            ShowHelp();
        }
        if (source_file == NULL) {
            sprintf(msgstr,"mp3cc: no source file\n");
            compileMSG();
            ShowHelp();
        }

    // --

		mem_init();
		initialize_compiler();
		units=malloc(8);
		*units=malloc(8);
		*units=NULL;
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
		//library_directory=output_directory;
		yyrestart(source_file);
		if ((yyin != NULL) && (output_path != NULL))
		{
            if (detect_units_only) {
                sprintf(msgstr,"Detecting units of \'%s\'...\n", source_file_name);
            } else {
                sprintf(msgstr,"Compiling \'%s\'...\n", source_file_name);
            }
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

    if (!detect_units_only)
    {
        switch(error_count)
        {
            case -1:
                ShowHelp();
            case 0:
                PrintFinalMessage();
            default:
                sprintf(msgstr,"");
        }
        compileMSG();
    }

	mem_close();
    return error_count;
}
