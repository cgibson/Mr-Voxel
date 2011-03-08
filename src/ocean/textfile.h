// textfile.h: interface for reading and writing text files
// www.lighthouse3d.com
//
// You may use these functions freely.
// they are provided as is, and no warranties, either implicit,
// or explicit are given
//////////////////////////////////////////////////////////////////////

#ifndef __TEXTFILE_H
#define __TEXTFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *textFileRead(const char *fn);
int textFileWrite(char *fn, char *s);

#endif
