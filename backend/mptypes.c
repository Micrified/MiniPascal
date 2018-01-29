#include "mptypes.h"

/*
********************************************************************************
*                          Internal DataType Functions                         *
********************************************************************************
*/

/* Safely allocates a new dataType. */
dataType *allocDataType (void) {
    dataType *dt;
    if ((dt = malloc(sizeof(dataType))) == NULL) {
        fprintf(stderr, "Error: allocDataType: Couldn't allocate memory!\n");
        exit(EXIT_FAILURE);
    }
    dt->id = dt->tc = dt->tt = dt->ti = dt->tn = dt->vb = dt->vl = dt->op = UNDEFINED;
    return dt;
}

/* Prints a given dataType. */
void printDataType (dataType *dt) {
    if (dt == NULL) {
        fprintf(stderr, "Null\n");
        return;
    }
    fprintf(stderr, "(dataType){\n\tid = %u\n\ttc = %u\n\ttt = %u\n\tti = %u\n\ttn = %u\n\tvb = %u\n\tvl = %u\n\top = %u\n}\n",
        dt->id, dt->tc, dt->tt, dt->ti, dt->tn, dt->vb, dt->vl, dt->op);
}

/*
********************************************************************************
*                               DataType Functions                             *
********************************************************************************
*/

/* Allocates a dataType for an expression constant. */
dataType *initExprConstDataType (unsigned tn, unsigned tt) {
    dataType *dt = allocDataType();
    dt->tn = tn;
    dt->tt = tt;
    return dt;
}

/* Allocates a dataType for an variable expression. */
dataType *initExprVarDataType (unsigned tn, unsigned tc, unsigned tt, unsigned id) {
    dataType *dt = allocDataType();
    dt->tn = tn;
    dt->tc = tc;
    dt->tt = tt;
    dt->id = id;
    return dt;
}

/* Allocates a dataType for a comparative expression */
dataType *initExprCompDataType (unsigned tn, unsigned op) {
    dataType *dt = allocDataType();
    dt->tt = TT_INTEGER;
    dt->tn = tn;
    dt->op = op;
    return dt;
}

/* Allocates a dataType for a scalar variable */
dataType *initVarDataType (unsigned tc, unsigned id) {
    dataType *dt = allocDataType();
    dt->tc = tc;
    dt->id = id;
    return dt;
}

/* Allocates a dataType for a indexed variable */
dataType *initVarIdxDataType (unsigned tc, unsigned id, unsigned ti) {
    dataType *dt = allocDataType();
    dt->tc = tc;
    dt->id = id;
    dt->ti = ti;
    return dt;
}

/* Free's an allocated dataType. */
void freeDataType (dataType *dt) {
    free(dt);
}

/*
********************************************************************************
*                             DataListType Functions                           *
********************************************************************************
*/

/* Initializes a new dataList. */
dataListType initDataListType (void) {
    return (dataListType){.length = 0, .list = NULL};
}

/* Frees allocated memory in a dataList */
void freeDataList(dataListType dataList) {
    for (int i = 0; i < dataList.length; i++) {
        freeDataType(dataList.list[i]);
    }
    free(dataList.list);
}

/* Allocates a new spot for the given dataType and inserts it into the dataList type. */
dataListType insertDataType (dataType *dt, dataListType dataList) {
    if ((dataList.list = realloc(dataList.list, (dataList.length + 1) * sizeof(dataType *))) == NULL) {
        fprintf(stderr, "Error: insertDataType: List reallocation failed!\n");
        exit(EXIT_FAILURE);
    }
    dataList.list[dataList.length++] = dt;
    return dataList;
}

/* Appends the first dataList to the second dataList. */
dataListType appendDataList (dataListType suffix, dataListType prefix) {

    // Compute new length of prefix.
    prefix.length += suffix.length;

    // Reallocate prefix list.
    if ((prefix.list = realloc(prefix.list, prefix.length * sizeof(dataType *))) == NULL) {
        fprintf(stderr, "Error: appendDataList: List reallocation failed!\n");
        exit(EXIT_FAILURE);
    }

    // Append suffix dataType pointers to prefix list.
    for (int i = 0; i < suffix.length; i++) {
        prefix.list[prefix.length - suffix.length + i] = suffix.list[i];
    }

    // Free suffix list.
    free(suffix.list);

    return prefix;
}

/* Maps a descriptor type to a list of dataTypes. Returns dataListType instance. */
dataListType mapDescToDataList (descType desc, dataListType dataList) {
    for (int i = 0; i < dataList.length; i++) {
        dataList.list[i]->tc = desc.tc;
        dataList.list[i]->tt = desc.tt;
        dataList.list[i]->vb = desc.vb;
        dataList.list[i]->vl = desc.vl;
    }
    return dataList;
}

/* Prints a dataListType */
void printDataListType (dataListType dataList) {
    fprintf(stdout, "(dataList) {\n\tlength = %u\nlist = \n", dataList.length);
    for (int i = 0; i < dataList.length; i++) {
        printDataType(dataList.list[i]);
    }
    fprintf(stdout, "- END OF LIST -\n");
}
