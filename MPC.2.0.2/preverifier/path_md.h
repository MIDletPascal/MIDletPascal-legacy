/*
 * @(#)path_md.h	1.4 02/09/27
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

#ifndef _PATH_MD_H_
#define _PATH_MD_H_

#define	DIR_SEPARATOR		'/'

#ifdef UNIX
#define	LOCAL_DIR_SEPARATOR		'/'
#define PATH_SEPARATOR          ':'

#include <dirent.h>
#endif
#ifdef WIN32

#define	LOCAL_DIR_SEPARATOR		'\\'
#define PATH_SEPARATOR          ';'

#include <direct.h>
struct dirent {
    char d_name[1024];
};

typedef struct {
    struct dirent dirent;
    char *path;
    HANDLE handle;
    WIN32_FIND_DATA find_data;
} DIR;

DIR *opendir(const char *dirname);
struct dirent *readdir(DIR *dirp);
int closedir(DIR *dirp);

#endif

#endif /* !_PATH_MD_H_ */
