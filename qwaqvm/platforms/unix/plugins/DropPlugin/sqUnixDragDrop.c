/* sqUnixDragDrop.c -- support for drag and drop, for those UIs that have it
 * 
 * Author: Ian Piumarta <ian.piumarta@inria.fr>
 * 
 *   Copyright (C) 1996-2004 by Ian Piumarta and other authors/contributors
 *                              listed elsewhere in this file.
 *   All rights reserved.
 *   
 *   This file is part of Unix Squeak.
 * 
 *   Permission is hereby granted, free of charge, to any person obtaining a
 *   copy of this software and associated documentation files (the "Software"),
 *   to deal in the Software without restriction, including without limitation
 *   the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *   and/or sell copies of the Software, and to permit persons to whom the
 *   Software is furnished to do so, subject to the following conditions:
 * 
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 * 
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *   DEALINGS IN THE SOFTWARE.
 */

/* Why on earth does this plugin exist at all?  Brain death strikes
 * again.  And why are half the functions never called from the VM?
 * Could it be that they are only there for certain ports (we'll
 * mention no names) in which the core support needs to grub around in
 * this plugin via ioLoadFunctionFromModule (and, putting disbelief
 * aside for a moment, maybe even VICE-VERSA)?  It seems to me that
 * truth is very definitely not beauty today.  Sigh...
 */


#include "sq.h"
#include "sqVirtualMachine.h"
#include "FilePlugin.h"
#include "DropPlugin.h"

#include <assert.h>


extern struct VirtualMachine  *interpreterProxy;
extern int    uxDropFileCount;
extern char **uxDropFileNames;
extern char  *dndNameIfActive(char *,int dropIndex);


int dropInit(void)	{ return 1; }
int dropShutdown(void)	{ return 1; }

char *dropRequestFileName(int dropIndex)	// in st coordinates
{
  if ((dropIndex > 0) && (dropIndex <= uxDropFileCount)) {
     /* Sigh; the image insists on a non-null string since it always sends
      * utf8ToString to the result of dropRequestFileName.  So we check for
      * validity below, /not/ here.
      */
      assert(uxDropFileNames);
      return uxDropFileNames[dropIndex - 1];
  }
  return 0;
}

int dropRequestFileHandle(int dropIndex)
{
  char *path= dropRequestFileName(dropIndex);

  /* Sigh; the image insists on a non-null string since it always sends
   * utf8ToString to the result of dropRequestFileName.  So here we check
   * for validity, not above.
   */
  if (path && dndNameIfActive(path,dropIndex)) {
      // you cannot be serious?
      int handle= instantiateClassindexableSize(classByteArray(), fileRecordSize());

      if (!failed()) {
		  sqFileOpen((SQFile *)fileValueOf(handle), (int)path, strlen(path), 0);
		  dndReceived(uxDropFileNames[dropIndex - 1]);
      }
      return handle;
  }  
  return interpreterProxy->nilObject();
}

int  sqSecFileAccessCallback(void *callback)		 { return 0; }
void sqSetNumberOfDropFiles(int numberOfFiles)		 { }
void sqSetFileInformation(int dropIndex, void *dropFile) { }
