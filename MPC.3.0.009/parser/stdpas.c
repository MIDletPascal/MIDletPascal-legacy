/********************************************************************
	
	stdpas.c - initializes the parser/semantic checker with a
	standard pascal types, constants etc.

  Niksa Orlic, 2004-04-29

********************************************************************/

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
//#include "../main/static_entry.h"

#include "../classgen/constant_pool.h"

#include "stdpas.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma warning (disable:4305)
#pragma warning (disable:4761)

int procedure_linenum;
extern char *source_file_name;
int usesForms = 0; /* set to 1 if FS.class should be added into the generated JAR file */
int usesSupport = 0; /* set to 1 if S.class should be added into the generated JAR file */
int usesFloat = 0; /* set to 1 if Float.class should be added into the generated JAR file */
int usesRecordStore = 0; /* set to 1 if RS.class should be added into the generated JAR file */
int usesHttp = 0; /* if H.class should be added */
int usesPlayer = 0; /* if P.class should be used */
int usesSMS = 0;	/* if uses SM.class */
extern int mathType;

//int usesRegisteredFeature = 0;

extern int canvasType;


/*
	Creates and initializes the root block
	with standard pascal types, constants etc.
*/
block* initialize_root_block()
{
	block *root_block;

	root_block = block_create(NULL, NULL);

	add_std_types(root_block);
	add_std_constants(root_block);
	add_std_functions(root_block);

	return root_block;
}


/*
	Adds the standard pascal types into the block.
*/
void add_std_types(block *item)
{
	add_std_type(item, integer_type, "integer");
	add_std_type(item, real_type, "real");
	add_std_type(item, boolean_type, "boolean");
	add_std_type(item, char_type, "char");
	add_std_type(item, string_type, "string");
	add_std_type(item, image_type, "image");
	add_std_type(item, command_type, "command");
	add_std_type(item, stream_type, "resource");
	add_std_type(item, record_store_type, "recordstore");
	add_std_type(item, http_type, "http");
}

/*
	Adds some constants into the block
*/
void add_std_constants(block *item)
{
	add_real_constant(item, (float) 3.1415926535897932384626433832795, "pi");

	/* key codes */
	add_integer_constant(item, 0, "KE_NONE");
	add_integer_constant(item, 0, "GA_NONE");

	add_integer_constant(item, 1, "GA_UP");
	add_integer_constant(item, 6, "GA_DOWN");
	add_integer_constant(item, 2, "GA_LEFT");
	add_integer_constant(item, 5, "GA_RIGHT");
	add_integer_constant(item, 8, "GA_FIRE");
	add_integer_constant(item, 9,  "GA_GAMEA");
	add_integer_constant(item, 10, "GA_GAMEB");
	add_integer_constant(item, 11, "GA_GAMEC");
	add_integer_constant(item, 12, "GA_GAMED");

	add_integer_constant(item, 48, "KE_KEY0");
	add_integer_constant(item, 49, "KE_KEY1");
	add_integer_constant(item, 50, "KE_KEY2");
	add_integer_constant(item, 51, "KE_KEY3");
	add_integer_constant(item, 52, "KE_KEY4");
	add_integer_constant(item, 53, "KE_KEY5");
	add_integer_constant(item, 54, "KE_KEY6");
	add_integer_constant(item, 55, "KE_KEY7");
	add_integer_constant(item, 56, "KE_KEY8");
	add_integer_constant(item, 57, "KE_KEY9");
	add_integer_constant(item, 42, "KE_STAR");
	add_integer_constant(item, 35, "KE_POUND");

	/* font constants */
	add_integer_constant(item, 0, "FONT_FACE_SYSTEM");
	add_integer_constant(item, 32, "FONT_FACE_MONOSPACE");
	add_integer_constant(item, 64, "FONT_FACE_PROPORTIONAL");

	add_integer_constant(item, 8, "FONT_SIZE_SMALL");
	add_integer_constant(item, 0, "FONT_SIZE_MEDIUM");
	add_integer_constant(item, 16, "FONT_SIZE_LARGE");

	add_integer_constant(item, 0, "FONT_STYLE_PLAIN");
	add_integer_constant(item, 1, "FONT_STYLE_BOLD");
	add_integer_constant(item, 2, "FONT_STYLE_ITALIC");
	add_integer_constant(item, 4, "FONT_STYLE_UNDERLINED");

	/* command constants */
	add_integer_constant(item, 1, "CM_SCREEN");
	add_integer_constant(item, 2, "CM_BACK");
	add_integer_constant(item, 3, "CM_CANCEL");
	add_integer_constant(item, 4, "CM_OK");
	add_integer_constant(item, 5, "CM_HELP");
	add_integer_constant(item, 6, "CM_STOP");
	add_integer_constant(item, 7, "CM_EXIT");
	add_integer_constant(item, 8, "CM_ITEM");

	/* text field constants */
	add_integer_constant(item, 0, "TF_ANY");
	add_integer_constant(item, 1, "TF_EMAIL");
	add_integer_constant(item, 2, "TF_NUMERIC");
	add_integer_constant(item, 3, "TF_PHONENUMBER");
	add_integer_constant(item, 4, "TF_URL");
	add_integer_constant(item,  0x10000, "TF_PASSWORD");

	/* choice constants */
	add_integer_constant(item, 1, "CH_EXCLUSIVE");
	add_integer_constant(item, 2, "CH_MULTIPLE");
	add_integer_constant(item, 3, "CH_IMPLICIT");

	/* http constants */
	add_string_constant(item, string_from_cstr("GET"), "GET");
	add_string_constant(item, string_from_cstr("HEAD"), "HEAD");
	add_string_constant(item, string_from_cstr("POST"), "POST");

	/* date field constants */
	add_integer_constant(item, 1, "DF_DATE");
	add_integer_constant(item, 2, "DF_TIME");
	add_integer_constant(item, 3, "DF_DATE_TIME");

	add_integer_constant(item, 1000, "EOF");
}


/*
	Adds standard functions and procedures
*/
void add_std_functions(block *item)
{

	add_special_function(item, real_type, real_type, "sqrt");
	add_special_function(item, real_type, integer_type, "trunc");
//	add_special_function(item, real_type, integer_type, "round");
	add_special_function(item, real_type, real_type, "sin");
	add_special_function(item, real_type, real_type, "cos");
	add_special_function(item, real_type, real_type, "atan");
	add_special_function(item, real_type, real_type, "log"); /* natural logarithm */
	add_special_function(item, real_type, real_type, "exp");
	add_special_function(item, real_type, real_type, "asin");
	add_special_function(item, real_type, real_type, "acos");
	add_special_function(item, real_type, real_type, "tan");	
	add_special_function(item, real_type, real_type, "toDegrees");
	add_special_function(item, real_type, real_type, "toRadians");
	add_special_function(item, real_type, real_type, "frac");
    add_special_function2(item, real_type, real_type, real_type, "atan2");
	add_special_function(item, real_type, real_type, "log10");
	add_special_function2(item, real_type, real_type, real_type, "pow");
	add_special_function2(item, string_type, integer_type, real_type, "stringToReal"); 
	add_special_function(item, real_type, real_type, "rabs");

	add_special_function(item, integer_type, integer_type, "sqr");
	add_special_function(item, integer_type, integer_type, "abs");

	add_special_function(item, integer_type, boolean_type, "odd");
	add_special_function(item, integer_type, char_type, "chr");
	add_special_function(item, char_type, integer_type, "ord");

	add_special_function(item, void_type, void_type, "halt");
	add_special_function(item, void_type, boolean_type, "isMidletPaused");
	
	/* RNG functions */
	add_special_function(item, void_type, void_type, "randomize");
	add_special_function(item, integer_type, integer_type, "random");

	/* string functions */
	add_special_function(item, string_type, string_type, "upcase");
	add_special_function(item, string_type, string_type, "locase");
	add_special_function(item, string_type, integer_type, "length");
	add_special_function3(item, string_type, integer_type, integer_type, string_type, "copy");
	add_special_function2(item, string_type, string_type, integer_type, "pos");
	add_special_function(item, string_type, integer_type, "stringToInteger");
	add_special_function(item, integer_type, string_type, "integerToString");
	add_special_function2(item, string_type, integer_type, char_type, "getChar");
	add_special_function3(item, string_type, char_type, integer_type, string_type, "setChar");

	/* midlet functions */
	add_special_function(item, string_type, string_type, "getproperty");


	/* time functions */
	add_special_function(item, integer_type, void_type, "delay");
	add_special_function(item, void_type, integer_type, "getRelativeTimeMs");
	add_special_function(item, void_type, integer_type, "getCurrentTime");
	add_special_function(item, integer_type, integer_type, "getDay");
	add_special_function(item, integer_type, integer_type, "getMonth");
	add_special_function(item, integer_type, integer_type, "getYear");
	add_special_function(item, integer_type, integer_type, "getSecond");
	add_special_function(item, integer_type, integer_type, "getMinute");
	add_special_function(item, integer_type, integer_type, "getHour");
	add_special_function(item, integer_type, integer_type, "getWeekDay");
	add_special_function(item, integer_type, integer_type, "getYearDay");


	/* music functions */
	add_special_function3(item, integer_type, integer_type, integer_type, void_type, "playTone");


	/* action functions */
	add_special_function(item, void_type, integer_type, "getKeyPressed");
	add_special_function(item, void_type, integer_type, "getKeyClicked");
	add_special_function(item, integer_type, integer_type, "keyToAction");


	/* image functions */
	add_special_function(item, string_type, image_type, "loadImage");
	add_special_function(item, image_type, integer_type, "getImageWidth");
	add_special_function(item, image_type, integer_type, "getImageHeight");
	add_special_function5(item, image_type, integer_type, integer_type, integer_type, integer_type, image_type, "imageFromImage");
	add_special_function4(item, integer_type, integer_type, integer_type, integer_type, image_type, "imageFromCanvas");
	add_special_function2(item, string_type, integer_type, image_type, "imageFromBuffer");

	/* drawing functions */
	add_special_function(item, void_type, void_type, "repaint");

	add_special_function(item, void_type, integer_type, "getWidth");
	add_special_function(item, void_type, integer_type, "getHeight");
	add_special_function(item, void_type, boolean_type, "isColorDisplay");
	add_special_function(item, void_type, integer_type, "getColorsNum");

	
	add_special_function3(item, string_type, integer_type, integer_type, void_type, "drawText");
	add_special_function4(item, integer_type, integer_type, integer_type, integer_type, void_type, "setClip");
	add_special_function4(item, integer_type, integer_type, integer_type, integer_type, void_type, "drawLine");
	add_special_function3(item, integer_type, integer_type, integer_type, void_type, "setColor");
	add_special_function4(item, integer_type, integer_type, integer_type, integer_type, void_type, "drawEllipse");
	add_special_function4(item, integer_type, integer_type, integer_type, integer_type, void_type, "fillEllipse");
	add_special_function4(item, integer_type, integer_type, integer_type, integer_type, void_type, "drawRect");
	add_special_function4(item, integer_type, integer_type, integer_type, integer_type, void_type, "fillRect");
	add_special_function6(item, integer_type, integer_type, integer_type, integer_type, integer_type, integer_type, void_type, "drawArc");
	add_special_function2(item, integer_type, integer_type, void_type, "plot");
	add_special_function(item, void_type, integer_type, "getColorRed");
	add_special_function(item, void_type, integer_type, "getColorGreen");
	add_special_function(item, void_type, integer_type, "getColorBlue");
	add_special_function6(item, integer_type, integer_type, integer_type, integer_type, integer_type, integer_type, void_type, "drawRoundRect");
	add_special_function6(item, integer_type, integer_type, integer_type, integer_type, integer_type, integer_type, void_type, "fillRoundRect");

	add_special_function3(item, integer_type, integer_type, integer_type, void_type, "setFont");
	add_special_function(item, void_type, void_type, "setDefaultFont");

	add_special_function3(item, image_type, integer_type, integer_type, void_type, "drawImage");

	add_special_function(item, string_type, integer_type, "getStringWidth");
	add_special_function(item, string_type, integer_type, "getStringHeight");

	/* debug functions */
	add_special_function(item, string_type, void_type, "debug");
	add_special_function(item, boolean_type, void_type, "assert");

	/* command & form functions */
	add_special_function3(item, string_type, integer_type, integer_type, command_type, "createCommand");
	add_special_function(item, void_type, command_type, "getClickedCommand");
	add_special_function(item, command_type, void_type, "addCommand");
	add_special_function(item, command_type, void_type, "removeCommand");
	add_special_function(item, void_type, command_type, "emptyCommand");

	add_special_function(item, void_type, void_type, "showForm");
	add_special_function(item, void_type, void_type, "showCanvas");

	add_special_function(item, string_type, void_type, "setTicker");

	add_special_function(item, void_type, void_type, "clearForm");
	add_special_function(item, integer_type, void_type, "formRemove");
	add_special_function(item, string_type, integer_type, "formAddString");
	add_special_function(item, void_type, integer_type, "formAddSpace");
	add_special_function(item, image_type, integer_type, "formAddImage");
	add_special_function4(item, string_type, string_type, integer_type, integer_type, integer_type, "formAddTextField");
	add_special_function4(item, string_type, boolean_type, integer_type, integer_type, integer_type, "formAddGauge");
	add_special_function2(item, string_type, integer_type, integer_type, "formAddDateField");
	add_special_function2(item, integer_type, integer_type, void_type, "formSetDate");
	add_special_function(item, integer_type, integer_type, "formGetDate");
	
	add_special_function2(item, string_type, integer_type, integer_type, "formAddChoice");
	
	add_special_function(item, integer_type, integer_type, "formGetValue");
	add_special_function(item, integer_type, string_type, "formGetText");
	add_special_function2(item, integer_type, integer_type, void_type, "formSetValue");
	add_special_function2(item, integer_type, string_type, void_type, "formSetText");

	add_special_function2(item, integer_type, string_type, integer_type, "choiceAppendString");
	add_special_function3(item, integer_type, string_type, image_type, integer_type, "choiceAppendStringImage");
	add_special_function2(item, integer_type, integer_type, boolean_type, "choiceIsSelected"); 
	add_special_function(item, integer_type, integer_type, "choiceGetSelectedIndex"); 

	add_special_function(item, string_type, void_type, "setFormTitle");
	add_special_function(item, void_type, void_type, "removeFormTitle");
	add_special_function(item, void_type, string_type, "getFormTitle");

	/* text box elements */
	add_special_function4(item, string_type, string_type, integer_type, integer_type, void_type, "showTextBox");
	add_special_function(item, void_type, string_type, "getTextBoxString");

	/* alert elements */
	add_special_function4(item, string_type, string_type, image_type, alert_type, void_type, "showAlert");
	add_special_function(item, void_type, void_type, "playAlertSound");
	add_special_function(item, void_type, alert_type, "ALERT_INFO");
	add_special_function(item, void_type, alert_type, "ALERT_WARNING");
	add_special_function(item, void_type, alert_type, "ALERT_ERROR");
	add_special_function(item, void_type, alert_type, "ALERT_ALARM");
	add_special_function(item, void_type, alert_type, "ALERT_CONFIRMATION");

	/* menu functions */
	add_special_function2(item, string_type, integer_type, void_type, "showMenu");
	add_special_function(item, string_type, integer_type, "menuAppendString");
	add_special_function2(item, string_type, image_type, integer_type, "menuAppendStringImage");
	add_special_function(item, integer_type, boolean_type, "menuIsSelected");
	add_special_function(item, void_type, integer_type, "menuGetSelectedIndex");


	/* record store functions */
	add_special_function(item, string_type, record_store_type, "openRecordStore");
	add_special_function(item, record_store_type, void_type, "closeRecordStore");
	add_special_function(item, string_type, void_type, "deleteRecordStore");
	add_special_function2(item, record_store_type, integer_type, void_type, "deleteRecordStoreEntry");
	add_special_function2(item, record_store_type, string_type, integer_type, "addRecordStoreEntry");
	add_special_function2(item, record_store_type, integer_type, string_type, "readRecordStoreEntry");
	add_special_function(item, record_store_type, integer_type, "getRecordStoreSize");
	add_special_function3(item, record_store_type, string_type, integer_type, void_type, "modifyRecordStoreEntry");
	add_special_function(item, record_store_type, integer_type, "getRecordStoreNextId");

	/* http connectivity functions */
	add_special_function2(item, http_type, string_type, boolean_type, "openHttp");
	add_special_function(item, http_type, boolean_type, "isHttpOpen");
	add_special_function(item, http_type, void_type, "closeHttp");
	add_special_function3(item, http_type, string_type, string_type, void_type, "addHttpHeader");
	add_special_function2(item, http_type, string_type, void_type, "setHttpMethod");
	add_special_function(item, http_type, integer_type, "sendHttpMessage");
	add_special_function2(item, http_type, string_type, string_type, "getHttpHeader");
	add_special_function2(item, http_type, string_type, void_type, "addHttpBody");	
	add_special_function(item, http_type, string_type, "getHttpResponse");


	/* resource-handling functions */
	add_special_function(item, string_type, stream_type, "openResource");
	add_special_function(item, stream_type, void_type, "closeResource");
	add_special_function(item, stream_type, integer_type, "readByte");
	add_special_function(item, stream_type, string_type, "readLine");
	add_special_function(item, stream_type, boolean_type, "resourceAvailable");

	/* player functions */
	add_special_function2(item, string_type, string_type, boolean_type, "openPlayer");
	add_special_function(item, void_type, boolean_type, "startPlayer");
	add_special_function(item, void_type, void_type, "stopPlayer");
	add_special_function(item, integer_type, boolean_type, "setPlayerCount");
	add_special_function(item, void_type, integer_type, "getPlayerDuration");


	/* SMS functions */
	add_special_function2(item, string_type, string_type, boolean_type, "smsStartSend");
	add_special_function(item, void_type, boolean_type, "smsIsSending");
	add_special_function(item, void_type, boolean_type, "smsWasSuccessfull");

}


/*
	Adds a single type into the block
*/
void add_std_type(block *item, enum en_type_class type_class, char *cstr_name)
{
	string *name;
	identifier *descriptor;

	name = string_from_cstr(cstr_name);
	descriptor = identifier_create();
	descriptor->identifier_class = type_name;
	descriptor->defined_type = type_create();
	descriptor->defined_type->type_class = type_class;
	name_table_insert(item->names, name, descriptor);
}


/*
	Add a standard function
*/
void add_std_function(block *item, char *cstr_name, type_list *params, type *return_type)
{
	string *name;
	identifier *descriptor;

	name = string_from_cstr(cstr_name);
	descriptor = identifier_create();
	descriptor->identifier_class = function_name;
	descriptor->return_type = return_type;
	descriptor->parameters = params;
	descriptor->variables = NULL;
	descriptor->standard_function = 1;
	name_table_insert(item->names, name, descriptor);
}


/*
	Add a standard procedure
*/
void add_std_procedure(block *item, char *cstr_name, type_list *params)
{
	string *name;
	identifier *descriptor;

	name = string_from_cstr(cstr_name);
	descriptor = identifier_create();
	descriptor->identifier_class = procedure_name;
	descriptor->parameters = params;
	descriptor->variables = NULL;
	descriptor->standard_function = 1;
	name_table_insert(item->names, name, descriptor);
}


/*
	Creates the 'prefix' code, the code that is executed before the arguments are evaluated
*/
void create_std_function_prefix(bytecode *code, char *name)
{
	lowercase(name);

	if ( (strcmp(name, "drawtext") == 0)
	  || (strcmp(name, "drawline") == 0)
	  || (strcmp(name, "setcolor") == 0)
	  || (strcmp(name, "drawellipse") == 0)
	  || (strcmp(name, "fillellipse") == 0)
	  || (strcmp(name, "drawrect") == 0)
	  || (strcmp(name, "fillrect") == 0)
	  || (strcmp(name, "drawarc") == 0)
	  || (strcmp(name, "plot") == 0)
	  || (strcmp(name, "getcolorred") == 0)
	  || (strcmp(name, "getcolorblue") == 0)
	  || (strcmp(name, "getcolorgreen") == 0)
	  || (strcmp(name, "drawroundrect") == 0)
	  || (strcmp(name, "fillroundrect") == 0)
	  || (strcmp(name, "setfont") == 0)
	  || (strcmp(name, "setdefaultfont") == 0)
	  || (strcmp(name, "drawimage") == 0)
	  || (strcmp(name, "getstringheight") == 0)
	  || (strcmp(name, "setclip") == 0)
	  )
	{
		int field_index = cp_add_fieldref("M", "G", "Ljavax/microedition/lcdui/Graphics;");
		
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, field_index);

		return;
	}

	if ( (strcmp(name, "getwidth") == 0)
	  || (strcmp(name, "getheight") == 0)
	  )
	{
		int field_index = cp_add_fieldref("M", "I", "Ljavax/microedition/lcdui/Image;");
		
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, field_index);

		return;
	}

	if ((strcmp(name, "getsecond") == 0)
		|| (strcmp(name, "getminute") == 0)
		|| (strcmp(name, "gethour") == 0)
		|| (strcmp(name, "getday") == 0)
		|| (strcmp(name, "getmonth") == 0)
		|| (strcmp(name, "getyear") == 0)
		|| (strcmp(name, "getweekday") == 0)
		|| (strcmp(name, "getyearday") == 0)		
		)
	{
		int method1, class1;
		method1 = cp_add_methodref("java/util/Calendar", "getInstance", "()Ljava/util/Calendar;");
		class1 = cp_add_class("java/util/Date");

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, method1);

		bytecode_append(code, dup$);

		bytecode_append(code, new$);
		bytecode_append_short_int(code, class1);

		bytecode_append(code, dup$); 
	
		return;
	}

	if (strcmp(name, "createcommand") == 0)
	{
		bytecode_append(code, new$);
		bytecode_append_short_int(code, cp_add_class("javax/microedition/lcdui/Command"));
		bytecode_append(code, dup$);
		
		return;
	}

	if ((strcmp(name, "vibrate") == 0)
		|| (strcmp(name, "iscolordisplay") == 0)
		|| (strcmp(name, "getcolorsnum") == 0))
	{
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "fw", "LFW;"));

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Display",
			"getDisplay", "(Ljavax/microedition/midlet/MIDlet;)Ljavax/microedition/lcdui/Display;"));
		return;
	}

	if (strcmp(name, "showtextbox") == 0)
	{
		bytecode_append(code, new$);
		bytecode_append_short_int(code, cp_add_class("javax/microedition/lcdui/TextBox"));
		bytecode_append(code, dup$);
		return;
	}

	if (strcmp(name, "showalert") == 0)
	{
		bytecode_append(code, new$);
		bytecode_append_short_int(code, cp_add_class("javax/microedition/lcdui/Alert"));
		bytecode_append(code, dup$);
		return;
	}

	if (strcmp(name, "showmenu") == 0)
	{
		bytecode_append(code, new$);
		bytecode_append_short_int(code, cp_add_class("javax/microedition/lcdui/List"));
		bytecode_append(code, dup$);
		return;
	}

	if ((strcmp(name, "menuappendstring") == 0)
		|| (strcmp(name, "menuappendstringimage") == 0)
		|| (strcmp(name, "menuisselected") == 0)
		|| (strcmp(name, "menugetselectedindex") == 0))
	{

		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "L", "Ljavax/microedition/lcdui/List;"));
		return;
	}

	if (strcmp(name, "stringtoreal") == 0)
	{
		if (mathType != 1)
		{
			bytecode_append(code, new$);
			bytecode_append_short_int(code, cp_add_class("Real"));
			bytecode_append(code, dup$);
		}
	}
}


/*
	Create the bytecode for the standard function or procedure
*/
void create_std_function_code(bytecode *code, char *name)
{
	int count = 0;

	lowercase(name);

	switch(name[0])
	{
	case 'a': goto letter_a;
	case 'b': goto letter_b;
	case 'c': goto letter_c;
	case 'd': goto letter_d;
	case 'e': goto letter_e;
	case 'f': goto letter_f;
	case 'g': goto letter_g;
	case 'h': goto letter_h;
	case 'i': goto letter_i;
	case 'j': goto letter_j;
	case 'k': goto letter_k;
	case 'l': goto letter_l;
	case 'm': goto letter_m;
	case 'n': goto letter_n;
	case 'o': goto letter_o;
	case 'p': goto letter_p;
	case 'q': goto letter_q;
	case 'r': goto letter_r;
	case 's': goto letter_s;
	case 't': goto letter_t;
	case 'u': goto letter_u;
	case 'v': goto letter_v;
	case 'w': goto letter_w;
	case 'x': goto letter_x;
	case 'y': goto letter_y;
	case 'z': goto letter_z;
	}

letter_a:
	if (strcmp(name, "addhttpbody") == 0)
	{
		usesHttp = 1;
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("H", "o", "(Ljava/lang/String;)I"));
		bytecode_append(code, pop$);
		return;
	}

	if (strcmp(name, "addcommand") == 0)
	{
		if (canvasType == FULL_NOKIA)
		{
			bytecode_append(code, getstatic$);
			bytecode_append_short_int(code, cp_add_fieldref("FW", "CD", "Ljavax/microedition/lcdui/Displayable;"));

			bytecode_append(code, dup$);
			bytecode_append(code, instanceof$);
			bytecode_append_short_int(code, cp_add_class("com/nokia/mid/ui/FullCanvas"));

			bytecode_append(code, ifne$);
			bytecode_append_short_int(code, 10);

			bytecode_append(code, swap$);
			bytecode_append(code, invokevirtual$);
			bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Displayable", "addCommand", "(Ljavax/microedition/lcdui/Command;)V"));
			bytecode_append(code, goto$);
			bytecode_append_short_int(code, 4);
			
			bytecode_append(code, pop2$);
		}
		else
		{
			bytecode_append(code, getstatic$);
			bytecode_append_short_int(code, cp_add_fieldref("FW", "CD", "Ljavax/microedition/lcdui/Displayable;"));
			bytecode_append(code, swap$);
			bytecode_append(code, invokevirtual$);
			bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Displayable", "addCommand", "(Ljavax/microedition/lcdui/Command;)V"));
		}

		return;
	}

	if (strcmp(name, "addrecordstoreentry") == 0)
	{
		usesRecordStore = 1;
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, 
			cp_add_methodref("RS", "L", "(Ljavax/microedition/rms/RecordStore;Ljava/lang/String;)I"));
		return;
	}

	if (strcmp(name, "assert") == 0)
	{
		char assert_text[128];
		sprintf(assert_text, "Assertion failed at: %s:%d", source_file_name ,procedure_linenum);
		bytecode_append(code, ifne$);
		bytecode_append_short_int(code, 12);

		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("java/lang/System", "out", "Ljava/io/PrintStream;"));
		bytecode_append(code, ldc_w$);
		bytecode_append_short_int(code, cp_add_string(assert_text));
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("java/io/PrintStream", "println", "(Ljava/lang/String;)V"));
		return;
	}

	if (strcmp(name, "abs") == 0)
	{
		int method_index;

		method_index = cp_add_methodref("java/lang/Math", "abs", "(I)I");

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, method_index);

		return;
	}

	if (strcmp(name, "atan") == 0)
	{
		usesFloat = 1;		
		if (mathType == 1)
		{
			bytecode_append(code, invokestatic$);
			bytecode_append_short_int(code, cp_add_methodref("F", "atan", "(I)I"));
		}
		else
		{
			bytecode_append(code, new$);
			bytecode_append_short_int(code, cp_add_class("Real"));
			bytecode_append(code, dup_x1$);
			bytecode_append(code, swap$);
			bytecode_append(code, invokespecial$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "<init>", "(LReal;)V"));
			bytecode_append(code, dup$);
			bytecode_append(code, invokevirtual$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "atan", "()V"));
		}		

		return;
	}

	if (strcmp(name, "atan2") == 0)
	{
		usesFloat = 1;
		if (mathType == 1)
		{
			bytecode_append(code, invokestatic$);
			bytecode_append_short_int(code, cp_add_methodref("F", "atan2", "(II)I"));
		}
		else
		{
			bytecode_append(code, new$);
			bytecode_append_short_int(code, cp_add_class("Real"));
			bytecode_append(code, dup_x1$);
			bytecode_append(code, swap$);
			bytecode_append(code, invokespecial$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "<init>", "(LReal;)V"));

			bytecode_append(code, swap$);

			bytecode_append(code, new$);
			bytecode_append_short_int(code, cp_add_class("Real"));
			bytecode_append(code, dup_x1$);
			bytecode_append(code, swap$);
			bytecode_append(code, invokespecial$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "<init>", "(LReal;)V"));

			bytecode_append(code, dup_x1$);
			bytecode_append(code, swap$);

			bytecode_append(code, invokevirtual$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "atan2", "(LReal;)V"));
		}

		return;
	}

	if (strcmp(name, "asin") == 0)
	{
		usesFloat = 1;
		if (mathType == 1)
		{
			bytecode_append(code, invokestatic$);
			bytecode_append_short_int(code, cp_add_methodref("F", "AS", "(I)I"));
		}
		else
		{
			bytecode_append(code, new$);
			bytecode_append_short_int(code, cp_add_class("Real"));
			bytecode_append(code, dup_x1$);
			bytecode_append(code, swap$);
			bytecode_append(code, invokespecial$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "<init>", "(LReal;)V"));
			bytecode_append(code, dup$);
			bytecode_append(code, invokevirtual$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "asin", "()V"));
		}

		return;
	}

	if (strcmp(name, "acos") == 0)
	{
		usesFloat = 1;
		if (mathType == 1)
		{
			bytecode_append(code, invokestatic$);
			bytecode_append_short_int(code, cp_add_methodref("F", "AC", "(I)I"));
		}
		else
		{
			bytecode_append(code, new$);
			bytecode_append_short_int(code, cp_add_class("Real"));
			bytecode_append(code, dup_x1$);
			bytecode_append(code, swap$);
			bytecode_append(code, invokespecial$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "<init>", "(LReal;)V"));
			bytecode_append(code, dup$);
			bytecode_append(code, invokevirtual$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "acos", "()V"));
		}

		return;
	}

	if (strcmp(name, "addhttpheader") == 0)
	{
		usesHttp = 1;
		//usesRegisteredFeature = 1;
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("H", "L", "(Ljava/lang/String;Ljava/lang/String;)V"));
		return;
	}

	if (strcmp(name, "alert_alarm") == 0)
	{
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("javax/microedition/lcdui/AlertType", 
														"ALARM", "Ljavax/microedition/lcdui/AlertType;"));
		return;
	}

	if (strcmp(name, "alert_confirmation") == 0)
	{
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("javax/microedition/lcdui/AlertType", 
														"CONFIRMATION", "Ljavax/microedition/lcdui/AlertType;"));
		return;
	}

	if (strcmp(name, "alert_error") == 0)
	{
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("javax/microedition/lcdui/AlertType", 
														"ERROR", "Ljavax/microedition/lcdui/AlertType;"));
		return;
	}

	if (strcmp(name, "alert_info") == 0)
	{
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("javax/microedition/lcdui/AlertType", 
														"INFO", "Ljavax/microedition/lcdui/AlertType;"));
		return;
	}

	if (strcmp(name, "alert_warning") == 0)
	{
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("javax/microedition/lcdui/AlertType", 
														"WARNING", "Ljavax/microedition/lcdui/AlertType;"));
		return;
	}

letter_b:
letter_c:	

	if (strcmp(name, "closeresource") == 0)
	{
		usesSupport = 1;
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("S", "r", "(Ljava/io/InputStream;)V"));
		return;
	}

	if (strcmp(name, "createcommand") == 0)
	{
		int method_index = cp_add_methodref("javax/microedition/lcdui/Command", "<init>", "(Ljava/lang/String;II)V");
		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, method_index);

		return;
	}

	if (strcmp(name, "cos") == 0)
	{
		usesFloat = 1;
		if (mathType == 1)
		{
			bytecode_append(code, invokestatic$);
			bytecode_append_short_int(code, cp_add_methodref("F", "C", "(I)I"));
		}
		else
		{
			bytecode_append(code, new$);
			bytecode_append_short_int(code, cp_add_class("Real"));
			bytecode_append(code, dup_x1$);
			bytecode_append(code, swap$);
			bytecode_append(code, invokespecial$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "<init>", "(LReal;)V"));
			bytecode_append(code, dup$);
			bytecode_append(code, invokevirtual$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "cos", "()V"));
		}

		return;
	}

	if (strcmp(name, "chr") == 0)
	{
		/* do nothing since we represent chars and integers internally in the same way */
		return;
	}

	if (strcmp(name, "copy") == 0)
	{
		int method_index;
		
		method_index = cp_add_methodref("java/lang/String", "substring", "(II)Ljava/lang/String;");
		
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);
		return;
	}

	if (strcmp(name, "clearform") == 0)
	{
		bytecode_append(code, new$);
		bytecode_append_short_int(code, cp_add_class("javax/microedition/lcdui/Form"));
		bytecode_append(code, dup$);

		bytecode_append(code, new$);
		bytecode_append_short_int(code, cp_add_class("java/lang/String"));
		bytecode_append(code, dup$);
		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, cp_add_methodref("java/lang/String", "<init>", "()V"));

		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Form", "<init>", "(Ljava/lang/String;)V"));

		bytecode_append(code, dup$);
		bytecode_append(code, putstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "F", "Ljavax/microedition/lcdui/Form;"));

		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "fw", "LFW;"));

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Displayable", "setCommandListener", "(Ljavax/microedition/lcdui/CommandListener;)V"));

		goto show_form;

		return;
	}

	if(strcmp(name, "choiceappendstring") == 0)
	{
		usesForms = 1;
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("FS", "I", "(ILjava/lang/String;)I"));
		return;
	}

	if(strcmp(name, "choiceappendstringimage") == 0)
	{
		usesForms = 1;
		//usesRegisteredFeature = 1;
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("FS", "I", "(ILjava/lang/String;Ljavax/microedition/lcdui/Image;)I"));
		return;
	}

	if(strcmp(name, "choiceisselected") == 0)
	{
		usesForms = 1;
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("FS", "L", "(II)I"));
		return;
	}

	if (strcmp(name, ""))

	if(strcmp(name, "choicegetselectedindex") == 0)
	{
		usesForms = 1;
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("FS", "L", "(I)I"));
		return;
	}

	if (strcmp(name, "closerecordstore") == 0)
	{
		usesRecordStore = 1;
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("RS", "L", "(Ljavax/microedition/rms/RecordStore;)V"));
		return;
	}

	if (strcmp(name, "closehttp") == 0)
	{
		usesHttp = 1;
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("H", "c", "()V"));
		return;
	}

letter_d:
	if (strcmp(name, "deleterecordstore") == 0)
	{
		usesRecordStore = 1;
		//usesRegisteredFeature = 1;
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("RS", "L", "(Ljava/lang/String;)V"));
		return;
	}

	if (strcmp(name, "deleterecordstoreentry") == 0)
	{
		usesRecordStore = 1;
		//usesRegisteredFeature = 1;
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code,
			cp_add_methodref("RS", "L", "(Ljavax/microedition/rms/RecordStore;I)V"));
		return;
	}

	if (strcmp(name, "debug") == 0)
	{
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("java/lang/System", "out", "Ljava/io/PrintStream;"));
		bytecode_append(code, swap$);
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("java/io/PrintStream", "println", "(Ljava/lang/String;)V"));
		return;
	}

	if (strcmp(name, "delay") == 0)
	{
		int method_index;

		method_index = cp_add_methodref("java/lang/Thread", "sleep", "(J)V");

		bytecode_append(code, i2l$);
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, method_index);

		return;
	}

	if (strcmp(name, "drawtext") == 0)
	{
		int method_index;

		method_index = cp_add_methodref("javax/microedition/lcdui/Graphics", "drawString", "(Ljava/lang/String;III)V");
		
		bytecode_append(code, bipush$);
		bytecode_append(code, 20);

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);

		return;
	}

	if (strcmp(name, "drawline") == 0)
	{
		int method_index;

		method_index = cp_add_methodref("javax/microedition/lcdui/Graphics", "drawLine", "(IIII)V");

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);

		return;
	}

	if (strcmp(name, "drawellipse") == 0)
	{
		int method_index;

		method_index = cp_add_methodref("javax/microedition/lcdui/Graphics", "drawArc", "(IIIIII)V");

		bytecode_append(code, bipush$);
		bytecode_append(code, 0);
		bytecode_append(code, sipush$);
		bytecode_append_short_int(code, 360);

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);

		return;
	}

	if (strcmp(name, "drawarc") == 0)
	{
		int method_index;

		method_index = cp_add_methodref("javax/microedition/lcdui/Graphics", "drawArc", "(IIIIII)V");

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);

		return;
	}

	if (strcmp(name, "drawrect") == 0)
	{
		int method_index;

		method_index = cp_add_methodref("javax/microedition/lcdui/Graphics", "drawRect", "(IIII)V");

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);

		return;
	}

	if (strcmp(name, "drawroundrect") == 0)
	{
		int method_index;

		method_index = cp_add_methodref("javax/microedition/lcdui/Graphics", "drawRoundRect", "(IIIIII)V");

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);

		return;
	}

	if (strcmp(name, "drawimage") == 0)
	{
		int method_index = cp_add_methodref("javax/microedition/lcdui/Graphics", "drawImage", "(Ljavax/microedition/lcdui/Image;III)V");

		bytecode_append(code, bipush$);
		bytecode_append(code, 20);

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);
		return;
	}

letter_e:
	if (strcmp(name, "emptycommand") == 0)
	{
		bytecode_append(code, aconst_null$);
		return;
	}

	if (strcmp(name, "exp") == 0)
	{
		usesFloat = 1;
		if (mathType == 1)
		{
			bytecode_append(code, invokestatic$);
			bytecode_append_short_int(code, cp_add_methodref("F", "e", "(I)I"));
		}
		else
		{
			bytecode_append(code, new$);
			bytecode_append_short_int(code, cp_add_class("Real"));
			bytecode_append(code, dup_x1$);
			bytecode_append(code, swap$);
			bytecode_append(code, invokespecial$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "<init>", "(LReal;)V"));
			bytecode_append(code, dup$);
			bytecode_append(code, invokevirtual$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "exp", "()V"));
		}

		return;
	}

letter_f:
	if (strcmp(name, "frac") == 0)
	{
		usesFloat = 1;
		if (mathType == 1)
		{
			bytecode_append(code, sipush$);
			bytecode_append_short_int(code, 0x0FFF);
			bytecode_append(code, iand$);
		}
		else
		{
			bytecode_append(code, new$);
			bytecode_append_short_int(code, cp_add_class("Real"));
			bytecode_append(code, dup_x1$);
			bytecode_append(code, swap$);
			bytecode_append(code, invokespecial$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "<init>", "(LReal;)V"));
			bytecode_append(code, dup$);
			bytecode_append(code, invokevirtual$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "frac", "()V"));
		}

		return;
	}

	if (strcmp(name, "fillellipse") == 0)
	{
		int method_index;

		method_index = cp_add_methodref("javax/microedition/lcdui/Graphics", "fillArc", "(IIIIII)V");

		bytecode_append(code, bipush$);
		bytecode_append(code, 0);
		bytecode_append(code, sipush$);
		bytecode_append_short_int(code, 360);

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);

		return;
	}

	if (strcmp(name, "fillrect") == 0)
	{
		int method_index;

		method_index = cp_add_methodref("javax/microedition/lcdui/Graphics", "fillRect", "(IIII)V");

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);

		return;
	}


	if (strcmp(name, "fillroundrect") == 0)
	{
		int method_index;

		method_index = cp_add_methodref("javax/microedition/lcdui/Graphics", "fillRoundRect", "(IIIIII)V");

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);

		return;
	}

	if (strcmp(name, "formaddstring") == 0)
	{
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "F", "Ljavax/microedition/lcdui/Form;"));

		bytecode_append(code, swap$);

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Form", "append", "(Ljava/lang/String;)I"));
		return;
	}

	if (strcmp(name, "formaddimage") == 0)
	{
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "F", "Ljavax/microedition/lcdui/Form;"));

		bytecode_append(code, swap$);

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Form", "append", "(Ljavax/microedition/lcdui/Image;)I"));
		return;
	}

	if (strcmp(name, "formadddatefield") == 0)
	{
		usesForms = 1;

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("FS", "dd", "(Ljava/lang/String;I)I"));
		return;
	}

	if (strcmp(name, "formsetdate") == 0)
	{
		usesForms = 1;

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("FS", "dd", "(II)V"));
		return;
	}

	if (strcmp(name, "formgetdate") == 0)
	{
		usesForms = 1;
		
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("FS", "dd", "(I)I"));
		return;
	}

	if (strcmp(name, "formaddspace") == 0)
	{
		/* get Form */
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "F", "Ljavax/microedition/lcdui/Form;"));

		/* new Spacer */
		bytecode_append(code, new$);
		bytecode_append_short_int(code, cp_add_class("javax/microedition/lcdui/Spacer"));
		
		bytecode_append(code, dup$);

		/* init spacer(10, 10)*/
		bytecode_append(code, bipush$);
		bytecode_append(code, 10);
		bytecode_append(code, dup$);
		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Spacer", "<init>", "(II)V"));

		/* form.append(Spacer)*/
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Form", "append", "(Ljavax/microedition/lcdui/Item;)I"));
		return;
	}

	if (strcmp(name, "formaddtextfield") == 0)
	{
		usesForms = 1;

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("FS", "Lj", "(Ljava/lang/String;Ljava/lang/String;II)I"));
		return;
	}

	if (strcmp(name, "formaddgauge") == 0)
	{
		usesForms = 1;

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("FS", "Lj", "(Ljava/lang/String;III)I"));
		return;
	}

	if (strcmp(name, "formremove") == 0)
	{
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "F", "Ljavax/microedition/lcdui/Form;"));

		bytecode_append(code, swap$);

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Form", "delete", "(I)V"));

		return;
	}

	if (strcmp(name, "formaddchoice") == 0)
	{
		usesForms = 1;

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("FS", "Lj", "(Ljava/lang/String;I)I"));
		return;
	}

	if (strcmp(name, "formgetvalue") == 0)
	{
		usesForms = 1;

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("FS", "Lja", "(I)I"));
		return;
	}

	if (strcmp(name, "formsetvalue") == 0)
	{
		usesForms = 1;

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("FS", "Lja", "(II)V"));
		return;
	}

	if (strcmp(name, "formgettext") == 0)
	{
		usesForms = 1;

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("FS", "ja", "(I)Ljava/lang/String;"));
		return;
	}

	if (strcmp(name, "formsettext") == 0)
	{
		usesForms = 1;

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("FS", "ja", "(ILjava/lang/String;)V"));
		return;
	}

letter_g:
	if (strcmp(name, "getformtitle") == 0)
	{
		usesForms = 1;

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("FS", "gft", "()Ljava/lang/String;"));
		return;
	}

	if (strcmp(name, "getrecordstorenextid") == 0)
	{
		usesRecordStore = 1;
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, 
			cp_add_methodref("RS", "Lja", "(Ljavax/microedition/rms/RecordStore;)I"));
		return;
	}

	if (strcmp(name, "getplayerduration") == 0)
	{
		usesPlayer = 1;

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("P", "c", "()I"));
		return;
	}

	if (strcmp(name, "getchar") == 0)
	{
		usesSupport = 1;

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("S", "gc", "(Ljava/lang/String;I)I"));
		return;
	}

	if (strcmp(name, "getrecordstoresize") == 0)
	{
		usesRecordStore = 1;
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("RS", "j", "(Ljavax/microedition/rms/RecordStore;)I"));
		return;
	}

	if (strcmp(name, "getclickedcommand") == 0)
	{
		/* return LC */
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "LC", "Ljavax/microedition/lcdui/Command;"));

		/* set LC = null */
		bytecode_append(code, aconst_null$);
		bytecode_append(code, putstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "LC", "Ljavax/microedition/lcdui/Command;"));
		return;
	}

	if (strcmp(name, "gettextboxstring") == 0)
	{
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "TB", "Ljavax/microedition/lcdui/TextBox;"));
		
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/TextBox", "getString", "()Ljava/lang/String;"));
		return;
	}

	if (strcmp(name, "getrelativetimems") == 0)
	{
		int method_index;

		method_index = cp_add_methodref("java/lang/System", "currentTimeMillis", "()J");
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, method_index);
		bytecode_append(code, l2i$);
		return;
	}

	if (strcmp(name, "getcurrenttime") == 0)
	{
		int method_index;

		method_index = cp_add_methodref("java/lang/System", "currentTimeMillis", "()J");
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, method_index);
		bytecode_append(code, sipush$);
		bytecode_append_short_int(code, 1000);
		bytecode_append(code, i2l$);
		bytecode_append(code, ldiv$);
		bytecode_append(code, l2i$);
		return;
	}

	if (strcmp(name, "getday") == 0)
	{
		int method1, method2, method3, method4;
		method1 = cp_add_methodref("java/util/Calendar", "getInstance", "()Ljava/util/Calendar;");
		method2 = cp_add_methodref("java/util/Calendar", "setTime", "(Ljava/util/Date;)V");
		method3 = cp_add_methodref("java/util/Calendar", "get", "(I)I");
		method4 = cp_add_methodref("java/util/Date", "<init>", "(J)V");

		/* time as int-seconds traslate into millis-long */
		bytecode_append(code, i2l$);
		bytecode_append(code, sipush$);
		bytecode_append_short_int(code, 1000);
		bytecode_append(code, i2l$);
		bytecode_append(code, lmul$);

		/* the stack is now: ..., Calendar, Calendar, Date, Date, Millis */
		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, method4);

		/* stack is now Calendar, Calendar, Date*/
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method2);

		bytecode_append(code, bipush$);
		bytecode_append(code, 5);

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method3);

		return;
	}

	if (strcmp(name, "getweekday") == 0)
	{
		int method1, method2, method3, method4;
		method1 = cp_add_methodref("java/util/Calendar", "getInstance", "()Ljava/util/Calendar;");
		method2 = cp_add_methodref("java/util/Calendar", "setTime", "(Ljava/util/Date;)V");
		method3 = cp_add_methodref("java/util/Calendar", "get", "(I)I");
		method4 = cp_add_methodref("java/util/Date", "<init>", "(J)V");

		/* time as int-seconds traslate into millis-long */
		bytecode_append(code, i2l$);
		bytecode_append(code, sipush$);
		bytecode_append_short_int(code, 1000);
		bytecode_append(code, i2l$);
		bytecode_append(code, lmul$);

		/* the stack is now: ..., Calendar, Calendar, Date, Date, Millis */
		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, method4);

		/* stack is now Calendar, Calendar, Date*/
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method2);

		bytecode_append(code, bipush$);
		bytecode_append(code, 7);

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method3);

		return;
	}

	if (strcmp(name, "getyearday") == 0)
	{
		int method1, method2, method3, method4;
		method1 = cp_add_methodref("java/util/Calendar", "getInstance", "()Ljava/util/Calendar;");
		method2 = cp_add_methodref("java/util/Calendar", "setTime", "(Ljava/util/Date;)V");
		method3 = cp_add_methodref("java/util/Calendar", "get", "(I)I");
		method4 = cp_add_methodref("java/util/Date", "<init>", "(J)V");

		/* time as int-seconds traslate into millis-long */
		bytecode_append(code, i2l$);
		bytecode_append(code, sipush$);
		bytecode_append_short_int(code, 1000);
		bytecode_append(code, i2l$);
		bytecode_append(code, lmul$);

		/* the stack is now: ..., Calendar, Calendar, Date, Date, Millis */
		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, method4);

		/* stack is now Calendar, Calendar, Date*/
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method2);

		bytecode_append(code, bipush$);
		bytecode_append(code, 6);

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method3);

		return;
	}

	if (strcmp(name, "getmonth") == 0)
	{
		int method1, method2, method3, method4;
		method1 = cp_add_methodref("java/util/Calendar", "getInstance", "()Ljava/util/Calendar;");
		method2 = cp_add_methodref("java/util/Calendar", "setTime", "(Ljava/util/Date;)V");
		method3 = cp_add_methodref("java/util/Calendar", "get", "(I)I");
		method4 = cp_add_methodref("java/util/Date", "<init>", "(J)V");

		/* time as int-seconds traslate into millis-long */
		bytecode_append(code, i2l$);
		bytecode_append(code, sipush$);
		bytecode_append_short_int(code, 1000);
		bytecode_append(code, i2l$);
		bytecode_append(code, lmul$);

		/* the stack is now: ..., Calendar, Calendar, Date, Date, Millis */
		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, method4);

		/* stack is now Calendar, Calendar, Date*/
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method2);

		bytecode_append(code, bipush$);
		bytecode_append(code, 2);

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method3);

		bytecode_append(code, iconst_0$);
		bytecode_append(code, iadd$);

		return;
	}

	if (strcmp(name, "getyear") == 0)
	{
		int method1, method2, method3, method4;
		method1 = cp_add_methodref("java/util/Calendar", "getInstance", "()Ljava/util/Calendar;");
		method2 = cp_add_methodref("java/util/Calendar", "setTime", "(Ljava/util/Date;)V");
		method3 = cp_add_methodref("java/util/Calendar", "get", "(I)I");
		method4 = cp_add_methodref("java/util/Date", "<init>", "(J)V");

		/* time as int-seconds traslate into millis-long */
		bytecode_append(code, i2l$);
		bytecode_append(code, sipush$);
		bytecode_append_short_int(code, 1000);
		bytecode_append(code, i2l$);
		bytecode_append(code, lmul$);

		/* the stack is now: ..., Calendar, Calendar, Date, Date, Millis */
		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, method4);

		/* stack is now Calendar, Calendar, Date*/
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method2);

		bytecode_append(code, bipush$);
		bytecode_append(code, 1);

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method3);

		return;
	}

	if (strcmp(name, "getsecond") == 0)
	{
		int method1, method2, method3, method4;
		method1 = cp_add_methodref("java/util/Calendar", "getInstance", "()Ljava/util/Calendar;");
		method2 = cp_add_methodref("java/util/Calendar", "setTime", "(Ljava/util/Date;)V");
		method3 = cp_add_methodref("java/util/Calendar", "get", "(I)I");
		method4 = cp_add_methodref("java/util/Date", "<init>", "(J)V");

		/* time as int-seconds traslate into millis-long */
		bytecode_append(code, i2l$);
		bytecode_append(code, sipush$);
		bytecode_append_short_int(code, 1000);
		bytecode_append(code, i2l$);
		bytecode_append(code, lmul$);

		/* the stack is now: ..., Calendar, Calendar, Date, Date, Millis */
		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, method4);

		/* stack is now Calendar, Calendar, Date*/
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method2);

		bytecode_append(code, bipush$);
		bytecode_append(code, 13);

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method3);

		return;
	}

	if (strcmp(name, "getminute") == 0)
	{
		int method1, method2, method3, method4;
		method1 = cp_add_methodref("java/util/Calendar", "getInstance", "()Ljava/util/Calendar;");
		method2 = cp_add_methodref("java/util/Calendar", "setTime", "(Ljava/util/Date;)V");
		method3 = cp_add_methodref("java/util/Calendar", "get", "(I)I");
		method4 = cp_add_methodref("java/util/Date", "<init>", "(J)V");

		/* time as int-seconds traslate into millis-long */
		bytecode_append(code, i2l$);
		bytecode_append(code, sipush$);
		bytecode_append_short_int(code, 1000);
		bytecode_append(code, i2l$);
		bytecode_append(code, lmul$);

		/* the stack is now: ..., Calendar, Calendar, Date, Date, Millis */
		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, method4);

		/* stack is now Calendar, Calendar, Date*/
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method2);

		bytecode_append(code, bipush$);
		bytecode_append(code, 12);

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method3);

		return;
	}

	if (strcmp(name, "gethour") == 0)
	{
		int method1, method2, method3, method4;
		method1 = cp_add_methodref("java/util/Calendar", "getInstance", "()Ljava/util/Calendar;");
		method2 = cp_add_methodref("java/util/Calendar", "setTime", "(Ljava/util/Date;)V");
		method3 = cp_add_methodref("java/util/Calendar", "get", "(I)I");
		method4 = cp_add_methodref("java/util/Date", "<init>", "(J)V");

		/* time as int-seconds traslate into millis-long */
		bytecode_append(code, i2l$);
		bytecode_append(code, sipush$);
		bytecode_append_short_int(code, 1000);
		bytecode_append(code, i2l$);
		bytecode_append(code, lmul$);

		/* the stack is now: ..., Calendar, Calendar, Date, Date, Millis */
		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, method4);

		/* stack is now Calendar, Calendar, Date*/
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method2);

		bytecode_append(code, bipush$);
		bytecode_append(code, 11);

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method3);

		return;
	}

	if (strcmp(name, "getkeypressed") == 0)
	{
		int field_index = cp_add_fieldref("M", "KP", "I");

		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, field_index);

		return;
	}

	if (strcmp(name, "getkeyclicked") == 0)
	{
		int field_index = cp_add_fieldref("M", "KC", "I");

		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, field_index);
		bytecode_append(code, iconst_0$);
		bytecode_append(code, putstatic$);
		bytecode_append_short_int(code, field_index);

		return;
	}

	if (strcmp(name, "getcolorred") == 0)
	{
		int method_index;

		method_index = cp_add_methodref("javax/microedition/lcdui/Graphics", "getRedComponent", "()I");

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);

		return;
	}

	if (strcmp(name, "getcolorgreen") == 0)
	{
		int method_index;

		method_index = cp_add_methodref("javax/microedition/lcdui/Graphics", "getGreenComponent", "()I");

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);

		return;
	}

	if (strcmp(name, "getcolorblue") == 0)
	{
		int method_index;

		method_index = cp_add_methodref("javax/microedition/lcdui/Graphics", "getBlueComponent", "()I");

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);

		return;
	}

	if (strcmp(name, "getwidth") == 0)
	{
		int method_index;

		method_index = cp_add_methodref("javax/microedition/lcdui/Image", "getWidth", "()I");

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);

		return;
	}

	if (strcmp(name, "getheight") == 0)
	{
		int method_index;

		method_index = cp_add_methodref("javax/microedition/lcdui/Image", "getHeight", "()I");

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);

		return;
	}

	if (strcmp(name, "getimagewidth") == 0)
	{
		int method_index = cp_add_methodref("javax/microedition/lcdui/Image", "getWidth", "()I");

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);
		return;
	}

	if (strcmp(name, "getimageheight") == 0)
	{
		int method_index = cp_add_methodref("javax/microedition/lcdui/Image", "getHeight", "()I");

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);
		return;
	}

	if (strcmp(name, "getstringheight") == 0)
	{
		int method_index;
		method_index = cp_add_methodref("javax/microedition/lcdui/Graphics", "getFont", "()Ljavax/microedition/lcdui/Font;");
		bytecode_append(code, pop$);
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);
		method_index = cp_add_methodref("javax/microedition/lcdui/Font", "getHeight", "()I");
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);
		return;
	}

	if (strcmp(name, "getstringwidth") == 0)
	{
		int method_index;
		int field_index = cp_add_fieldref("M", "G", "Ljavax/microedition/lcdui/Graphics;");
		
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, field_index);

		method_index = cp_add_methodref("javax/microedition/lcdui/Graphics", "getFont", "()Ljavax/microedition/lcdui/Font;");
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);

		bytecode_append(code, swap$);
		method_index = cp_add_methodref("javax/microedition/lcdui/Font", "stringWidth", "(Ljava/lang/String;)I");
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);

		return;
	}

	if (strcmp(name, "getproperty") == 0)
	{
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("java/lang/System", "getProperty", "(Ljava/lang/String;)Ljava/lang/String;"));
		bytecode_append(code, dup$);
		bytecode_append(code, ifnonnull$);
		bytecode_append_short_int(code, 11);

		/* if the returned string is null, create an empty string */
		bytecode_append(code, pop$);
		bytecode_append(code, new$);
		bytecode_append_short_int(code, cp_add_class("java/lang/String"));
		bytecode_append(code, dup$);
		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, cp_add_methodref("java/lang/String", "<init>", "()V"));
		return;
	}

	if (strcmp(name, "getcolorsnum") == 0)
	{
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Display", "numColors", "()I"));
		return;
	}

	if (strcmp(name, "gethttpheader") == 0)
	{
		usesHttp = 1;
		//usesRegisteredFeature = 1;
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("H", "i", "(Ljava/lang/String;)Ljava/lang/String;"));
		return;
	}

	if (strcmp(name, "gethttpresponse") == 0)
	{
		usesHttp = 1;
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("H", "j", "()Ljava/lang/String;"));
		return;
	}

letter_h:
	if (strcmp(name, "halt") == 0)
	{
		int field_index;
		int method_index;

		field_index = cp_add_fieldref("FW", "fw", "LFW;");
		method_index = cp_add_methodref("FW", "destroyApp", "(Z)V");

		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, field_index);
		bytecode_append(code, iconst_1$);
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("java/lang/Thread", "currentThread", "()Ljava/lang/Thread;"));

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("java/lang/Thread", "join", "()V"));

		bytecode_append(code, sipush$);
		bytecode_append_short_int(code, 1000);
		bytecode_append(code, i2l$);
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("java/lang/Thread", "sleep", "(J)V"));

		return;
	}

letter_i:
	if (strcmp(name, "imagefromimage") == 0)
	{
		usesSupport = 1;

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("S", "ii", "(Ljavax/microedition/lcdui/Image;IIII)Ljavax/microedition/lcdui/Image;"));
		return;
	}

	if (strcmp(name, "imagefromcanvas") == 0)
	{
		usesSupport = 1;

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("S", "ii", "(IIII)Ljavax/microedition/lcdui/Image;"));
		return;
	}

	if (strcmp(name, "imagefrombuffer") == 0)
	{
		usesSupport = 1;

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("S", "ii", "(Ljava/lang/String;I)Ljavax/microedition/lcdui/Image;"));
		return;
	}

	if (strcmp(name, "integertostring") == 0)
	{
		bytecode_append(code, new$);
		bytecode_append_short_int(code, cp_add_class("java/lang/StringBuffer"));
		bytecode_append(code, dup$);
		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, cp_add_methodref("java/lang/StringBuffer", "<init>", "()V"));

		bytecode_append(code, swap$);

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("java/lang/StringBuffer", "append", "(I)Ljava/lang/StringBuffer;"));

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("java/lang/StringBuffer", "toString", "()Ljava/lang/String;"));
		return;
	}

	if (strcmp(name, "ismidletpaused") == 0)
	{
		int field_index = cp_add_fieldref("FW", "MP", "I");

		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, field_index);

		return;
	}

	if (strcmp(name, "ishttpopen") == 0)
	{
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("H", "L", "()I"));
		return;
	}

	if (strcmp(name, "iscolordisplay") == 0)
	{
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Display", "isColor", "()Z"));

		bytecode_append(code, ifeq$);
		bytecode_append_short_int(code, 7);

		bytecode_append(code, iconst_m1$);
		bytecode_append(code, goto$);
		bytecode_append_short_int(code, 4);

		bytecode_append(code, iconst_0$);

		return;
	}

letter_j:
letter_k:
	if (strcmp(name, "keytoaction") == 0)
	{
		int field_index = cp_add_fieldref("M", "T", "LM;");

		/* check if the argument is zero, goto (1)*/
		bytecode_append(code, dup$);
		bytecode_append(code, ifeq$);
		bytecode_append_short_int(code, 13);

		/* otherwise call the getGameAction*/
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, field_index);
		bytecode_append(code, swap$);
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("M", "getGameAction", "(I)I"));

		/* goto (2) */
		bytecode_append(code, goto$);
		bytecode_append_short_int(code, 5);

		/* (1): pop the operands, push 0 to the stack */
		bytecode_append(code, pop$);
		bytecode_append(code, iconst_0$);

		/* (2): continue */

		return;
	}

letter_l:
	if (strcmp(name, "log") == 0)
	{
		usesFloat = 1;
		if (mathType == 1)
		{
			bytecode_append(code, invokestatic$);
			bytecode_append_short_int(code, cp_add_methodref("F", "log", "(I)I"));
		}
		else
		{
			bytecode_append(code, new$);
			bytecode_append_short_int(code, cp_add_class("Real"));
			bytecode_append(code, dup_x1$);
			bytecode_append(code, swap$);
			bytecode_append(code, invokespecial$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "<init>", "(LReal;)V"));
			bytecode_append(code, dup$);
			bytecode_append(code, invokevirtual$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "ln", "()V"));
		}

		return;
	}

	if (strcmp(name, "log10") == 0)
	{
		usesFloat = 1;
		if (mathType == 1)
		{
			bytecode_append(code, invokestatic$);
			bytecode_append_short_int(code, cp_add_methodref("F", "log10", "(I)I"));
		}
		else
		{
			bytecode_append(code, new$);
			bytecode_append_short_int(code, cp_add_class("Real"));
			bytecode_append(code, dup_x1$);
			bytecode_append(code, swap$);
			bytecode_append(code, invokespecial$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "<init>", "(LReal;)V"));
			bytecode_append(code, dup$);
			bytecode_append(code, invokevirtual$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "log10", "()V"));
		}

		return;
	}

	if (strcmp(name, "length") == 0)
	{
		int method_index = cp_add_methodref("java/lang/String", "length", "()I");
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);
		return;
	}

	if (strcmp(name, "locase") == 0)
	{
		int init_index;
		int class_index;
		int method_index;
		
		class_index = cp_add_class("java/lang/String");
		init_index = cp_add_methodref("java/lang/String", "<init>", "(Ljava/lang/String;)V");
		method_index = cp_add_methodref("java/lang/String", "toLowerCase", "()Ljava/lang/String;");

		bytecode_append(code, new$);
		bytecode_append_short_int(code, class_index);
		bytecode_append(code, dup_x1$);
		bytecode_append(code, swap$);
		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, init_index);

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);
		return;
	}

	if (strcmp(name, "loadimage") == 0)
	{
		int method_index = cp_add_methodref("javax/microedition/lcdui/Image", "createImage", "(Ljava/lang/String;)Ljavax/microedition/lcdui/Image;");

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, method_index);
		return;
	}

letter_m:
	if (strcmp(name, "modifyrecordstoreentry") == 0)
	{
		usesRecordStore = 1;
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, 
			cp_add_methodref("RS", "L", "(Ljavax/microedition/rms/RecordStore;Ljava/lang/String;I)V"));
		return;
	}


	if (strcmp(name, "menuappendstring") == 0)
	{
		bytecode_append(code, aconst_null$);

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/List",
														 "append",
														 "(Ljava/lang/String;Ljavax/microedition/lcdui/Image;)I"));
		return;
	}

	if (strcmp(name, "menuappendstringimage") == 0)
	{
		//usesRegisteredFeature = 1;
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/List",
														 "append",
														 "(Ljava/lang/String;Ljavax/microedition/lcdui/Image;)I"));
		return;
	}

	if (strcmp(name, "menuisselected") == 0)
	{
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/List",
														 "isSelected",
														 "(I)Z"));

		bytecode_append(code, ifeq$);
		bytecode_append_short_int(code, 7);
		bytecode_append(code, iconst_m1$);
		bytecode_append(code, goto$);
		bytecode_append_short_int(code, 4);
		bytecode_append(code, iconst_0$);

		return;
	}

	if (strcmp(name, "menugetselectedindex") == 0)
	{
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/List",
														 "getSelectedIndex",
														 "()I"));
		return;
	}



letter_n:
letter_o:

	if (strcmp(name, "openplayer") == 0)
	{
		usesPlayer = 1;
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("P", "a", "(Ljava/lang/String;Ljava/lang/String;)I"));
		return;
	}

	if (strcmp(name, "openresource") == 0)
	{
		usesSupport = 1;
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("S", "r", "(Ljava/lang/String;)Ljava/io/InputStream;"));
		return;
	}

	if (strcmp(name, "ord") == 0)
	{
		/* do nothing, opposite of chr */
		return;
	}

	if (strcmp(name, "openhttp") == 0)
	{
		usesHttp = 1;
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("H", "L", "(Ljava/lang/String;)I"));
		return;
	}

	if (strcmp(name, "odd") == 0)
	{
		/* just leave the last bit: if it is zero, the number is even and the 0 (false)
		   will be on the stack */
		bytecode_append(code, iconst_1$);
		bytecode_append(code, iand$);
		bytecode_append(code, iconst_m1$);
		bytecode_append(code, imul$);

		return;
	}

	if (strcmp(name, "openrecordstore") == 0)
	{
		usesRecordStore = 1;

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("RS", "j", "(Ljava/lang/String;)Ljavax/microedition/rms/RecordStore;"));
		return;
	}

letter_p:
	if (strcmp(name, "playtone") == 0)
	{
		int method_index = cp_add_methodref("javax/microedition/media/Manager", "playTone", "(III)V");

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, method_index);

		return;
	}

	if (strcmp(name, "playalertsound") == 0)
	{
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "A", "Ljavax/microedition/lcdui/Alert;"));

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Alert", "getType", "()Ljavax/microedition/lcdui/AlertType;"));

		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "fw", "LFW;"));

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Display", "getDisplay", "(Ljavax/microedition/midlet/MIDlet;)Ljavax/microedition/lcdui/Display;"));

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref(
			"javax/microedition/lcdui/AlertType", 
			"playSound",
			"(Ljavax/microedition/lcdui/Display;)Z"));

		bytecode_append(code, pop$);
		return;
	}

	if (strcmp(name, "pow") == 0)
	{
		usesFloat = 1;
		if (mathType == 1)
		{
			bytecode_append(code, invokestatic$);
			bytecode_append_short_int(code, cp_add_methodref("F", "p", "(II)I"));
		}
		else
		{
			bytecode_append(code, new$);
			bytecode_append_short_int(code, cp_add_class("Real"));
			bytecode_append(code, dup_x1$);
			bytecode_append(code, swap$);
			bytecode_append(code, invokespecial$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "<init>", "(LReal;)V"));

			bytecode_append(code, swap$);

			bytecode_append(code, new$);
			bytecode_append_short_int(code, cp_add_class("Real"));
			bytecode_append(code, dup_x1$);
			bytecode_append(code, swap$);
			bytecode_append(code, invokespecial$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "<init>", "(LReal;)V"));

			bytecode_append(code, dup_x1$);
			bytecode_append(code, swap$);

			bytecode_append(code, invokevirtual$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "pow", "(LReal;)V"));
		}
	
		return;
	}

	if (strcmp(name, "pos") == 0)
	{
		int method_index;
		
		method_index = cp_add_methodref("java/lang/String", "indexOf", "(Ljava/lang/String;)I");
		
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);
		return;
	}

	if (strcmp(name, "plot") == 0)
	{
		int method_index;

		method_index = cp_add_methodref("javax/microedition/lcdui/Graphics", "fillRect", "(IIII)V");

		bytecode_append(code, iconst_1$);
		bytecode_append(code, dup$);

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);

		return;
	}

letter_q:
letter_r:
	if (strcmp(name, "removeformtitle") == 0)
	{
		/* get Form */
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "F", "Ljavax/microedition/lcdui/Form;"));

		/* set the form's title to null */
		bytecode_append(code, aconst_null$);
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Form", "setTitle", "(Ljava/lang/String;)V"));

		return;
	}

	if (strcmp(name, "resourceavailable") == 0)
	{
		bytecode_append(code, aconst_null$);
		bytecode_append(code, if_acmpeq$);
		bytecode_append_short_int(code, 7);

		bytecode_append(code, iconst_m1$);
		bytecode_append(code, goto$);
		bytecode_append_short_int(code, 4);

		bytecode_append(code, iconst_0$);
		return;
	}

	if (strcmp(name, "readbyte") == 0)
	{
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("S", "rb", "(Ljava/io/InputStream;)I"));
		return;
	}

	if (strcmp(name, "readline") == 0)
	{
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("S", "nl", "(Ljava/io/InputStream;)Ljava/lang/String;"));
		return;
	}

	if (strcmp(name, "readrecordstoreentry") == 0)
	{
		usesRecordStore = 1;

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("RS", "j", "(Ljavax/microedition/rms/RecordStore;I)Ljava/lang/String;"));
		return;
	}

	if (strcmp(name, "round") == 0)
	{
		int method_index;

		method_index = cp_add_methodref("java/lang/Math", "round", "(F)I");

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, method_index);

		return;
	}

	if (strcmp(name, "rabs") == 0)
	{
		usesFloat = 1;
		if (mathType == 1)
		{
			bytecode_append(code, invokestatic$);
			bytecode_append_short_int(code, cp_add_methodref("F", "A", "(I)I"));
		}
		else
		{
			bytecode_append(code, new$);
			bytecode_append_short_int(code, cp_add_class("Real"));
			bytecode_append(code, dup_x1$);
			bytecode_append(code, swap$);
			bytecode_append(code, invokespecial$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "<init>", "(LReal;)V"));
			bytecode_append(code, dup$);
			bytecode_append(code, invokevirtual$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "abs", "()V"));
		}

		return;
	}

	if (strcmp(name, "randomize") == 0)
	{
		/* re-initialize the random number generator */
		int random_field_index;
		int random_class_index;
		int random_method_index;

		random_field_index = cp_add_fieldref("M", "RNG", "Ljava/util/Random;");
		random_class_index = cp_add_class("java/util/Random");
		random_method_index = cp_add_methodref("java/util/Random", "<init>", "()V");

		bytecode_append(code, new$);
		bytecode_append_short_int(code, random_class_index);

		bytecode_append(code, dup$);

		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, random_method_index);

		bytecode_append(code, putstatic$);
		bytecode_append_short_int(code, random_field_index);
		return;
	}

	if (strcmp(name, "random") == 0)
	{
		int rng_index;
		int method_index;

		rng_index = cp_add_fieldref("M", "RNG", "Ljava/util/Random;");
		method_index = cp_add_methodref("java/util/Random", "nextInt", "()I");

		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, rng_index);

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);

		// do abs
		bytecode_append(code, iconst_m1$);
		bytecode_append(code, iconst_1$);
		bytecode_append(code, iushr$);
		bytecode_append(code, iand$);

		bytecode_append(code, swap$);
		bytecode_append(code, irem$);

		return;
	}

	if (strcmp(name, "removecommand") == 0)
	{
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "CD", "Ljavax/microedition/lcdui/Displayable;"));
		bytecode_append(code, swap$);
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Displayable", "removeCommand", "(Ljavax/microedition/lcdui/Command;)V"));
		return;
	}


	if (strcmp(name, "repaint") == 0)
	{
		int method_index;
		int field_index;

		method_index = cp_add_methodref("M", "repaint", "()V");
		field_index = cp_add_fieldref("M", "T", "LM;");

		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, field_index);

		bytecode_append(code, dup$);

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);

		method_index = cp_add_methodref("M", "serviceRepaints", "()V");
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);

		return;
	}

letter_s:
	if (strcmp(name, "setformtitle") == 0)
	{
		/* get Form */
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "F", "Ljavax/microedition/lcdui/Form;"));

		/* set the form's title */
		bytecode_append(code, swap$);
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Form", "setTitle", "(Ljava/lang/String;)V"));

		return;
	}

	if (strcmp(name, "setclip") == 0)
	{
		int method_index;

		method_index = cp_add_methodref("javax/microedition/lcdui/Graphics", "setClip", "(IIII)V");
				
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);

		return;
	}

	if (strcmp("smsstartsend", name) == 0)
	{
		usesSMS = 1;
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("SM", "send", "(Ljava/lang/String;Ljava/lang/String;)I"));
		return;
	}

	if (strcmp("smsissending", name) == 0)
	{
		usesSMS = 1;
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("SM", "IS", "()I"));
		return;
	}

	if (strcmp("smswassuccessfull", name) == 0)
	{
		usesSMS = 1;
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("SM", "GS", "()I"));
		return;
	}

	if (strcmp("startplayer", name) == 0)
	{
		usesPlayer = 1;
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("P", "a", "()I"));
		return;
	}

	if (strcmp("stopplayer", name) == 0)
	{
		usesPlayer = 1;
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("P", "b", "()V"));
		return;
	}

	if (strcmp("setplayercount", name) == 0)
	{
		usesPlayer = 1;
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("P", "a", "(I)I"));
		return;
	}

	if (strcmp(name, "setchar") == 0)
	{
		usesSupport = 1;

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("S", "gc", "(Ljava/lang/String;II)Ljava/lang/String;"));
		return;
	}

	if (strcmp(name, "sqr") == 0)
	{
		bytecode_append(code, dup$);
		bytecode_append(code, imul$);

		return;
	}

	if (strcmp(name, "setticker") == 0)
	{
		bytecode_append(code, new$);
		bytecode_append_short_int(code, cp_add_class("javax/microedition/lcdui/Ticker"));
		bytecode_append(code, dup_x1$);
		bytecode_append(code, swap$);
		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Ticker", "<init>", "(Ljava/lang/String;)V"));

		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "F", "Ljavax/microedition/lcdui/Form;"));

		bytecode_append(code, swap$);

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Form", "setTicker", "(Ljavax/microedition/lcdui/Ticker;)V"));
		return;
	}

	if (strcmp(name, "stringtointeger") == 0)
	{
		usesSupport = 1;
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("S", "parseInt", "(Ljava/lang/String;)I"));
		return;
	}

	if (strcmp(name, "stringtoreal") == 0)
	{
		usesFloat = 1;
		//usesRegisteredFeature = 1;
		if (mathType == 1)
		{
			bytecode_append(code, invokestatic$);
			bytecode_append_short_int(code, cp_add_methodref("F", "fI", "(Ljava/lang/String;I)I"));
		}
		else
		{
			bytecode_append(code, invokespecial$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "<init>", "(Ljava/lang/String;I)V"));
		}

		return;
	}


	if (strcmp(name, "sqrt") == 0)
	{
		usesFloat = 1;
		if (mathType == 1)
		{
			bytecode_append(code, invokestatic$);
			bytecode_append_short_int(code, cp_add_methodref("F", "S", "(I)I"));
		}
		else
		{
			bytecode_append(code, new$);
			bytecode_append_short_int(code, cp_add_class("Real"));
			bytecode_append(code, dup_x1$);
			bytecode_append(code, swap$);
			bytecode_append(code, invokespecial$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "<init>", "(LReal;)V"));
			bytecode_append(code, dup$);
			bytecode_append(code, invokevirtual$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "sqrt", "()V"));
		}

		return;
	}

	if (strcmp(name, "sin") == 0)
	{
		usesFloat = 1;
		if (mathType == 1)
		{
			bytecode_append(code, invokestatic$);
			bytecode_append_short_int(code, cp_add_methodref("F", "s", "(I)I"));
		}
		else
		{
			bytecode_append(code, new$);
			bytecode_append_short_int(code, cp_add_class("Real"));
			bytecode_append(code, dup_x1$);
			bytecode_append(code, swap$);
			bytecode_append(code, invokespecial$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "<init>", "(LReal;)V"));
			bytecode_append(code, dup$);
			bytecode_append(code, invokevirtual$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "sin", "()V"));
		}

		return;
	}

	if (strcmp(name, "setcolor") == 0)
	{
		int method_index;

		method_index = cp_add_methodref("javax/microedition/lcdui/Graphics", "setColor", "(III)V");

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);

		return;
	}

	

	if (strcmp(name, "setfont") == 0)
	{
		int method_index = cp_add_methodref("javax/microedition/lcdui/Font", "getFont", "(III)Ljavax/microedition/lcdui/Font;");

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, method_index);

		method_index = cp_add_methodref("javax/microedition/lcdui/Graphics", "setFont", "(Ljavax/microedition/lcdui/Font;)V");
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);

		return;
	}


	if (strcmp(name, "setdefaultfont") == 0)
	{
		int method_index = cp_add_methodref("javax/microedition/lcdui/Font", "getDefaultFont", "()Ljavax/microedition/lcdui/Font;");

		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, method_index);

		method_index = cp_add_methodref("javax/microedition/lcdui/Graphics", "setFont", "(Ljavax/microedition/lcdui/Font;)V");
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);

		return;
	}

	if (strcmp(name, "sethttpmethod") == 0)
	{
		usesHttp = 1;
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("H", "c", "(Ljava/lang/String;)V"));
		return;
	}

	if (strcmp(name, "sendhttpmessage") == 0)
	{
		usesHttp = 1;
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("H", "o", "()I"));
		return;
	}

	if (strcmp(name, "showform") == 0)
	{
show_form:
		/*
			Display.getDisplay(FW.fw).setCurrent(FW.F);
		*/
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "fw", "LFW;"));
		
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Display", "getDisplay", "(Ljavax/microedition/midlet/MIDlet;)Ljavax/microedition/lcdui/Display;"));

		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "F", "Ljavax/microedition/lcdui/Form;"));

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Display", "setCurrent", "(Ljavax/microedition/lcdui/Displayable;)V"));

		/*
			FW.CD = FW.F;
		*/
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "F", "Ljavax/microedition/lcdui/Form;"));
		
		bytecode_append(code, putstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "CD", "Ljavax/microedition/lcdui/Displayable;"));
		
		return;
	}

	if (strcmp(name, "showcanvas") == 0)
	{
		/*
			Display.getDisplay(FW.fw).setCurrent(M.T);
		*/
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "fw", "LFW;"));
		
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Display", "getDisplay", "(Ljavax/microedition/midlet/MIDlet;)Ljavax/microedition/lcdui/Display;"));

		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("M", "T", "LM;"));

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Display", "setCurrent", "(Ljavax/microedition/lcdui/Displayable;)V"));

		/*
			FW.CD = M.T;
		*/
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("M", "T", "LM;"));
		
		bytecode_append(code, putstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "CD", "Ljavax/microedition/lcdui/Displayable;"));
		
		return;
	}


	if (strcmp(name, "showtextbox") == 0)
	{
		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/TextBox", "<init>", "(Ljava/lang/String;Ljava/lang/String;II)V"));

		bytecode_append(code, putstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "TB", "Ljavax/microedition/lcdui/TextBox;"));

		/*
			Display.getDisplay(FW.fw).setCurrent(FW.TB);
		*/
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "fw", "LFW;"));
		
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Display", "getDisplay", "(Ljavax/microedition/midlet/MIDlet;)Ljavax/microedition/lcdui/Display;"));

		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "TB", "Ljavax/microedition/lcdui/TextBox;"));

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Display", "setCurrent", "(Ljavax/microedition/lcdui/Displayable;)V"));

		/*
			FW.CD = FW.TB;
		*/
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "TB", "Ljavax/microedition/lcdui/TextBox;"));
		
		bytecode_append(code, putstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "CD", "Ljavax/microedition/lcdui/Displayable;"));


		/*
			FW.TB.setCommandListener(FW.fw);
		*/
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "TB", "Ljavax/microedition/lcdui/TextBox;"));

		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "fw", "LFW;"));

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Displayable", "setCommandListener", "(Ljavax/microedition/lcdui/CommandListener;)V"));

		return;
	}

	if (strcmp(name, "showmenu") == 0)
	{
		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/List", "<init>", "(Ljava/lang/String;I)V"));

		bytecode_append(code, putstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "L", "Ljavax/microedition/lcdui/List;"));

		/*
			Display.getDisplay(FW.fw).setCurrent(FW.L);
		*/
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "fw", "LFW;"));
		
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Display", "getDisplay", "(Ljavax/microedition/midlet/MIDlet;)Ljavax/microedition/lcdui/Display;"));

		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "L", "Ljavax/microedition/lcdui/List;"));

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Display", "setCurrent", "(Ljavax/microedition/lcdui/Displayable;)V"));

		/*
			FW.CD = FW.L;
		*/
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "L", "Ljavax/microedition/lcdui/List;"));
		
		bytecode_append(code, putstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "CD", "Ljavax/microedition/lcdui/Displayable;"));


		/*
			FW.L.setCommandListener(FW.fw);
		*/
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "L", "Ljavax/microedition/lcdui/List;"));

		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "fw", "LFW;"));

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Displayable", "setCommandListener", "(Ljavax/microedition/lcdui/CommandListener;)V"));


		return;
	}


	if (strcmp(name, "showalert") == 0)
	{
		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Alert", "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljavax/microedition/lcdui/Image;Ljavax/microedition/lcdui/AlertType;)V"));

		bytecode_append(code, putstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "A", "Ljavax/microedition/lcdui/Alert;"));

		/*
			Display.getDisplay(FW.fw).setCurrent(FW.A);
		*/
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "fw", "LFW;"));
		
		bytecode_append(code, invokestatic$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Display", "getDisplay", "(Ljavax/microedition/midlet/MIDlet;)Ljavax/microedition/lcdui/Display;"));

		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "A", "Ljavax/microedition/lcdui/Alert;"));

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Display", "setCurrent", "(Ljavax/microedition/lcdui/Displayable;)V"));

		/*
			FW.CD = FW.A;
		*/
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "A", "Ljavax/microedition/lcdui/Alert;"));
		
		bytecode_append(code, putstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "CD", "Ljavax/microedition/lcdui/Displayable;"));

		/*
			FW.A.setCommandListener(FW.fw);
		*/
		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "A", "Ljavax/microedition/lcdui/Alert;"));

		bytecode_append(code, getstatic$);
		bytecode_append_short_int(code, cp_add_fieldref("FW", "fw", "LFW;"));

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Alert", "setCommandListener", "(Ljavax/microedition/lcdui/CommandListener;)V"));


		return;
	}

letter_t:
	if (strcmp(name, "trunc") == 0)
	{
		usesFloat = 1;
		if (mathType == 1)
		{
			bytecode_append(code, invokestatic$);
			bytecode_append_short_int(code, cp_add_methodref("F", "tI", "(I)I"));
		}
		else
		{
			bytecode_append(code, new$);
			bytecode_append_short_int(code, cp_add_class("Real"));
			bytecode_append(code, dup_x1$);
			bytecode_append(code, swap$);
			bytecode_append(code, invokespecial$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "<init>", "(LReal;)V"));
			bytecode_append(code, dup$);
			bytecode_append(code, invokevirtual$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "trunc", "()V"));
			bytecode_append(code, invokevirtual$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "toInteger", "()I"));
		}

		return;
	}
	

	if (strcmp(name, "tan") == 0)
	{
		usesFloat = 1;
		if (mathType == 1)
		{
			bytecode_append(code, invokestatic$);
			bytecode_append_short_int(code, cp_add_methodref("F", "tan", "(I)I"));
		}
		else
		{
			bytecode_append(code, new$);
			bytecode_append_short_int(code, cp_add_class("Real"));
			bytecode_append(code, dup_x1$);
			bytecode_append(code, swap$);
			bytecode_append(code, invokespecial$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "<init>", "(LReal;)V"));
			bytecode_append(code, dup$);
			bytecode_append(code, invokevirtual$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "tan", "()V"));
		}

		return;
	}



	if (strcmp(name, "todegrees") == 0)
	{
		usesFloat = 1;
		if (mathType == 1)
		{
			bytecode_append(code, invokestatic$);
			bytecode_append_short_int(code, cp_add_methodref("F", "tD", "(I)I"));
		}
		else
		{
			bytecode_append(code, new$);
			bytecode_append_short_int(code, cp_add_class("Real"));
			bytecode_append(code, dup_x1$);
			bytecode_append(code, swap$);
			bytecode_append(code, invokespecial$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "<init>", "(LReal;)V"));

			bytecode_append(code, dup$);
			bytecode_append(code, dup$);
			
			bytecode_append(code, sipush$);
			bytecode_append_short_int(code, 180);

			bytecode_append(code, invokevirtual$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "mul", "(I)V"));

			bytecode_append(code, getstatic$);
			bytecode_append_short_int(code, cp_add_fieldref("Real", "PI", "LReal;"));

			bytecode_append(code, invokevirtual$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "div", "(LReal;)V"));
		}

		return;
	}

	if (strcmp(name, "toradians") == 0)
	{
		usesFloat = 1;
		if (mathType == 1)
		{
			bytecode_append(code, invokestatic$);
			bytecode_append_short_int(code, cp_add_methodref("F", "tR", "(I)I"));
		}
		else
		{
			bytecode_append(code, new$);
			bytecode_append_short_int(code, cp_add_class("Real"));
			bytecode_append(code, dup_x1$);
			bytecode_append(code, swap$);
			bytecode_append(code, invokespecial$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "<init>", "(LReal;)V"));

			bytecode_append(code, dup$);
			bytecode_append(code, dup$);
			
			bytecode_append(code, sipush$);
			bytecode_append_short_int(code, 180);

			bytecode_append(code, invokevirtual$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "div", "(I)V"));

			bytecode_append(code, getstatic$);
			bytecode_append_short_int(code, cp_add_fieldref("Real", "PI", "LReal;"));

			bytecode_append(code, invokevirtual$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "mul", "(LReal;)V"));
		}

		return;
	}
letter_u:
	if (strcmp(name, "upcase") == 0)
	{
		int init_index;
		int class_index;
		int method_index;
		
		class_index = cp_add_class("java/lang/String");
		init_index = cp_add_methodref("java/lang/String", "<init>", "(Ljava/lang/String;)V");
		method_index = cp_add_methodref("java/lang/String", "toUpperCase", "()Ljava/lang/String;");

		bytecode_append(code, new$);
		bytecode_append_short_int(code, class_index);
		bytecode_append(code, dup_x1$);
		bytecode_append(code, swap$);
		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, init_index);

		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, method_index);
		return;
	}

letter_v:
	if (strcmp(name, "vibrate") == 0)
	{
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Display", "vibrate", "(I)Z"));

		bytecode_append(code, pop$);
		return;
	}
letter_w:
letter_x:
letter_y:
letter_z:
	
	if (count == 0)
	{
		count ++;
		goto letter_a;
	}

	die(25);
}


/*
	Adds a function with 0 or 1 parameter
*/
void add_special_function(block *item, enum en_type_class parameter_en, enum en_type_class return_type_en, char *name)
{
	type_list *params;
	type *param;
	type *return_type;

	params = type_list_create();
	param = type_create();
	return_type = type_create();
	param->type_class = parameter_en;
	return_type->type_class = return_type_en;
	if (parameter_en != void_type)
		type_list_append(params, param);
	type_destroy(param);
	if (return_type_en == void_type)
		add_std_procedure(item, name, params);
	else
		add_std_function(item, name, params, return_type);
}

/*
	Adds a function with 2 parameters
*/
void add_special_function2(block *item, enum en_type_class parameter_en1, enum en_type_class parameter_en2, enum en_type_class return_type_en, char *name)
{
	type_list *params;
	type *param;
	type *return_type;

	params = type_list_create();
	param = type_create();
	return_type = type_create();
	param->type_class = parameter_en1;
	return_type->type_class = return_type_en;	
	type_list_append(params, param);
	param->type_class = parameter_en2;
	type_list_append(params, param);
	type_destroy(param);
	
	if (return_type_en == void_type)
		add_std_procedure(item, name, params);
	else
		add_std_function(item, name, params, return_type);
}

/*
	Adds a function with 3 parameters
*/
void add_special_function3(block *item, enum en_type_class parameter_en1, enum en_type_class parameter_en2, enum en_type_class parameter_en3, enum en_type_class return_type_en, char *name)
{
	type_list *params;
	type *param;
	type *return_type;

	params = type_list_create();
	param = type_create();
	return_type = type_create();
	param->type_class = parameter_en1;
	return_type->type_class = return_type_en;	
	type_list_append(params, param);
	param->type_class = parameter_en2;
	type_list_append(params, param);
	param->type_class = parameter_en3;
	type_list_append(params, param);
	type_destroy(param);
	
	if (return_type_en == void_type)
		add_std_procedure(item, name, params);
	else
		add_std_function(item, name, params, return_type);
}

/*
	Adds a function with 4 parameters
*/
void add_special_function4(block *item, enum en_type_class parameter_en1, enum en_type_class parameter_en2, enum en_type_class parameter_en3, enum en_type_class parameter_en4, enum en_type_class return_type_en, char *name)
{
	type_list *params;
	type *param;
	type *return_type;

	params = type_list_create();
	param = type_create();
	return_type = type_create();
	param->type_class = parameter_en1;
	return_type->type_class = return_type_en;	
	type_list_append(params, param);
	param->type_class = parameter_en2;
	type_list_append(params, param);
	param->type_class = parameter_en3;
	type_list_append(params, param);
		param->type_class = parameter_en4;
	type_list_append(params, param);
	type_destroy(param);
	
	if (return_type_en == void_type)
		add_std_procedure(item, name, params);
	else
		add_std_function(item, name, params, return_type);
}

/*
	Adds a function with 5 parameters
*/
void add_special_function5(block *item, enum en_type_class parameter_en1, enum en_type_class parameter_en2, enum en_type_class parameter_en3, enum en_type_class parameter_en4, enum en_type_class parameter_en5, enum en_type_class return_type_en, char *name)
{
	type_list *params;
	type *param;
	type *return_type;

	params = type_list_create();
	param = type_create();
	return_type = type_create();
	param->type_class = parameter_en1;
	return_type->type_class = return_type_en;	
	type_list_append(params, param);
	param->type_class = parameter_en2;
	type_list_append(params, param);
	param->type_class = parameter_en3;
	type_list_append(params, param);
	param->type_class = parameter_en4;
	type_list_append(params, param);
	param->type_class = parameter_en5;
	type_list_append(params, param);
	type_destroy(param);
	
	if (return_type_en == void_type)
		add_std_procedure(item, name, params);
	else
		add_std_function(item, name, params, return_type);
}

/*
	Adds a function with 6 parameters
*/
void add_special_function6(block *item, enum en_type_class parameter_en1, enum en_type_class parameter_en2, enum en_type_class parameter_en3, enum en_type_class parameter_en4, enum en_type_class parameter_en5, enum en_type_class parameter_en6, enum en_type_class return_type_en, char *name)
{
	type_list *params;
	type *param;
	type *return_type;

	params = type_list_create();
	param = type_create();
	return_type = type_create();
	param->type_class = parameter_en1;
	return_type->type_class = return_type_en;	
	type_list_append(params, param);
	param->type_class = parameter_en2;
	type_list_append(params, param);
	param->type_class = parameter_en3;
	type_list_append(params, param);
	param->type_class = parameter_en4;
	type_list_append(params, param);
	param->type_class = parameter_en5;
	type_list_append(params, param);
	param->type_class = parameter_en6;
	type_list_append(params, param);
	type_destroy(param);
	
	if (return_type_en == void_type)
		add_std_procedure(item, name, params);
	else
		add_std_function(item, name, params, return_type);
}