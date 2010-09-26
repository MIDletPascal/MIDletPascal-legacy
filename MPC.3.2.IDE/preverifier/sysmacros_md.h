/*
 * @(#)sysmacros_md.h	1.5 02/09/27
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

#ifndef _SYSMACROS_MD_H_
#define _SYSMACROS_MD_H_

/*
 * Because these are used directly as function ptrs, just redefine the name
 */
#define sysMalloc	malloc
#define sysFree		free
#define sysCalloc	calloc
#define sysRealloc	realloc

/* A macro for sneaking into a sys_mon_t to get the owner sys_thread_t */
#define sysMonitorOwner(mid)   ((mid)->monitor_owner)

#ifdef DEBUG
extern void DumpThreads(void);
void panic (const char *, ...);
#define sysAssert(expression) {		\
    if (!(expression)) {		\
	DumpThreads();			\
	panic("\"%s\", line %d: assertion failure\n", __FILE__, __LINE__); \
    }					\
}
#else
#define sysAssert(expression) 
#endif

/*
 * Case insensitive compare of ASCII strings
 */
#define sysStricmp(a, b)		strcasecmp(a, b)

/*
 * File system macros
 */
#ifdef UNIX
#define sysOpen(_path, _oflag, _mode)	open(_path, _oflag, _mode)
#define sysNativePath(path)	        (path) 
#endif
#ifdef WIN32
#include <io.h>
#define sysOpen(_path, _oflag, _mode)	open(_path, _oflag | O_BINARY, _mode)
char *sysNativePath(char *);
#endif
#define sysRead(_fd, _buf, _n)		read(_fd, _buf, _n)
#define sysWrite(_fd, _buf, _n)		write(_fd, _buf, _n)
#define sysClose(_fd)			close(_fd)
#define sysAccess(_path, _mode)		access(_path, _mode)
#define sysStat(_path, _buf)		stat(_path, _buf)
#define sysMkdir(_path, _mode)		mkdir(_path, _mode)
#define sysUnlink(_path)		unlink(_path)
#define sysIsAbsolute(_path)		(*(_path) == '/')
#define sysCloseDir(_dir)		closedir(_dir)
#define sysOpenDir(_path)		opendir(_path)
#define sysRmdir(_dir)                  remove(_dir)
#define sysSeek(fd, offset, whence)	lseek(fd, offset, whence)
#define sysRename(s, d)			rename(s, d)

#endif /*_SYSMACROS_MD_H_*/
