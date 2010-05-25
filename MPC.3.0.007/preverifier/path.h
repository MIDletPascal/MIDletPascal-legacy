/*
 * @(#)path.h	1.6 02/09/27
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

#ifndef	_PATH_H_
#define	_PATH_H_

#include <sys/stat.h>
#include "path_md.h"

typedef struct {
    struct {
        unsigned long locpos;
	unsigned long cenpos;
    } jar;
    char type;
    char name[1];
} zip_t; 

/*
 * Class path element, which is either a directory or zip file.
 */
typedef struct {
    enum { CPE_DIR, CPE_ZIP } type;
    union {
	zip_t *zip;
	char *dir;
    } u;
} cpe_t;

cpe_t **sysGetClassPath(void);
void pushDirectoryOntoClassPath(char* directory);
void pushJarFileOntoClassPath(zip_t *zip);
void popClassPath(void);

bool_t 
findJARDirectories(zip_t *entry, struct stat *statbuf);

#endif	/* !_PATH_H_ */
