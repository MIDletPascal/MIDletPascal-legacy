***********************************************************************
*                                                                     *
* Important information for building lexical scanner from tl.lex file *
*                                                                     *
*                                            Niksa Orlic, Sept. 2003. *
*                                                                     *
***********************************************************************


Step 1.
-------
I used flex version 2.5.2 for generating lexical scanner. I ran it as 
following:

flex -olex.yy.cpp tl.lex

so the generated scanner has .cpp extension instead of .c


Step 2.
-------
Apply this step ONLY if compiling with Microsoft Visual C++ compiler!!!

Unfortunatly, generated scanner has one #ifdef _cplusplus directive
which tells the compiler to include <unistd.h> header file. This
will work on UNIX (and similar) machines, but MSVC does not have
such include file (unistd = UNIx STanDard, I guess). So, if you are
compiling on Micro$oft Visual C++ compiler, you should open generated 
lex.yy.cpp file and do the following:

	1. Delete all lines from 22 to 41, including lines 22 and 41
	   (the line 22 contains '#ifdef __cplusplus' and the line
	   41 contains '#endif	/* ! __cplusplus */')
           
	   NOTE that line numbers may vary between different flex
	        versions!!!
          

	2. Insert the following code instead of deleted lines:

		#include <stdio.h>
		#include <io.h>		/* isatty() define */
		#define YY_USE_CONST
		#define YY_USE_PROTOS