/*
 * @(#)stubs.c	1.6 02/09/27
 *
 * Copyright 1995-1998 by Sun Microsystems, Inc.,
 * 901 San Antonio Road, Palo Alto, California, 94303, U.S.A.
 * All rights reserved.
 * 
 * This software is the confidential and proprietary information
 * of Sun Microsystems, Inc. ("Confidential Information").  You
 * shall not disclose such Confidential Information and shall use
 * it only in accordance with the terms of the license agreement
 * you entered into with Sun.
 * Use is subject to license terms.
 */

/*=========================================================================
 * SYSTEM:    Verifier
 * SUBSYSTEM: Stubs.
 * FILE:      stubs.c
 * OVERVIEW:  Miscellaneous functions used during class loading, etc.
 *
 * AUTHOR:    Sheng Liang, Sun Microsystems, Inc.
 *            Edited by Tasneem Sayeed, Sun Microsystems
 *=======================================================================*/

/*=========================================================================
 * Include files
 *=======================================================================*/

#include "oobj.h"
#include "sys_api.h"

void
SignalError(struct execenv * ee, char *ename, char *DetailMessage)
{
	printCurrentClassName();
    jio_fprintf(stderr, "%s", ename);
	if (DetailMessage) {
		jio_fprintf(stderr, ": %s\n", DetailMessage);
	} else {
		jio_fprintf(stderr, "\n");
	}
    exit(1);
}

bool_t
VerifyClassAccess(ClassClass *current_class, ClassClass *new_class, 
		  bool_t classloader_only) 
{
    return TRUE;
}

/* This is called by FindClassFromClass when a class name is being requested
 * by another class that was loaded via a classloader.  For javah, we don't
 * really care.
 */

ClassClass *
ClassLoaderFindClass(struct execenv *ee, 
		     struct Hjava_lang_ClassLoader *loader, 
		     char *name, bool_t resolve)
{ 
    return NULL;
}

/* Get the execution environment
 */
ExecEnv *
EE() {
    static struct execenv lee;
    return &lee;
}

bool_t RunStaticInitializers(ClassClass *cb)
{
    return TRUE;
}

void InitializeInvoker(ClassClass *cb)
{
}

int verifyclasses = VERIFY_ALL;
long nbinclasses, sizebinclasses;
ClassClass **binclasses;
bool_t verbose;
struct StrIDhash *nameTypeHash;
struct StrIDhash *stringHash;

void *allocClassClass()
{
    ClassClass *cb = sysCalloc(1, sizeof(ClassClass));
    Classjava_lang_Class *ucb = sysCalloc(1, sizeof(Classjava_lang_Class));
    
    cb->obj = ucb;
    ucb->HandleToSelf = cb;
    return cb;
}

void DumpThreads() {}
