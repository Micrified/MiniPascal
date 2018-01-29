#include "mpio.h"

/*
***************************************************************************
*                  Internal Symbolic Constants & Variables
***************************************************************************
*/

/* Default IR file header */
#define MPIR_FILE_HEADER        "#include <stdio.h>\n"

/*
***************************************************************************
*                           Internal Routines
***************************************************************************
*/



/*
***************************************************************************
*                           Routine Prototypes
***************************************************************************
*/

/* Opens and formats a file for IR generation. Returns nonzero on error. */
int openIRFile (const char *filename) {
    if (filename == NULL || (irfp = fopen(filename, "w")) == NULL) {
        return 1;
    }
    fprintf(irfp, MPIR_FILE_HEADER);
    return 0;
}

/* Closes any open writable file. */
void closeIRFile (void) {
    if (irfp != NULL) {
        fclose(irfp);
        return;
    }
    fprintf(stderr, "Warning: closeWritableIRFile: Already closed!\n");
}