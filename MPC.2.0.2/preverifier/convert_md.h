/*
 * @(#)convert_md.h    1.2 00/11/22
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

#ifndef _CONVERT_MD_
#define _CONVERT_MD_

extern int native2utf8(const char* from, char* to, int buflen);
extern int utf2native(const char* from, char* to, int buflen);

#endif
