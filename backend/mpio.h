#if !defined(MPIO_H)
#define MPIO_H

#include <stdio.h>
#include <stdlib.h>

/*
***************************************************************************
*                               MiniPasal I/O                             *
* AUTHORS: Charles Randolph, Joe Jones.                                   *
* SNUMBERS: s2897318, s2990652.                                           *
***************************************************************************
*/

/*
***************************************************************************
*                  Symbolic Constants & Global Variables
***************************************************************************
*/

// Default filename for generated intermediate-representation C file.
#define MPIR_DEFAULT_FILENAME   "mpp.c"

/* The writable file pointer */
FILE *irfp;

/*
***************************************************************************
*                           Routine Prototypes
***************************************************************************
*/

/* Opens and formats a file for IR generation. Returns nonzero on error. */
int openIRFile (const char *filename);

/* Closes any open writable file. */
void closeIRFile (void);

#endif