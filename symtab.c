#include "symtab.h"

/*
********************************************************************************
*                  Internal Data Structures & Symbolic Constants               *
********************************************************************************
*/

// Elementary linked-list structure.
typedef struct node {
    IdEntry *entry;
    struct node *next;
} Node;

// Prime symbol-table size.
#define SYMTAB_SIZE     6959

// Symbol-table levels.
#define SYMTAB_LVLS     2

// Symbol table.
Node *symTable[SYMTAB_SIZE][SYMTAB_LVLS];

// Table scope level.
unsigned lvl;

/*
********************************************************************************
*                           Internal IdEntry Routines                          *
********************************************************************************
*/

/* Safely allocates an IdEntry instance and returns its pointer. */
static IdEntry *allocateIdEntry (void) {
    IdEntry *entry;
    if ((entry = malloc(sizeof(IdEntry))) == NULL) {
        fprintf(stderr, "Error: allocateIdEntry: Allocation failure!\n");
        exit(EXIT_FAILURE);
    }
    return entry;
}

/* Frees a IdData instance */
static void freeIdData (IdData *data) {
    for (int i = 0; i < data->argc; i++) {
        free(data->argv[i]);
    }
    free(data->argv);
}

/* Frees an IdEntry instance */
static void freeIdEntry (IdEntry *entry) {

    if (entry == NULL) {
        return;
    }

    if (entry->tc == TC_ROUTINE) {
        freeIdData(&(entry->data));
    }

    free(entry);
}

/*
********************************************************************************
*                          Internal Linked List Routines                       *
********************************************************************************
*/

/* Allocates new list-node and returns its pointer. Returns NULL on error. */
static Node *newNode (IdEntry *entry, Node *next) {
    Node *n = NULL;
    if ((n = malloc(sizeof(Node))) == NULL) {
        fprintf(stderr, "Error: newNode: Couldn't initialize new list node!\n");
        exit(EXIT_FAILURE);
    }
    n->entry = entry;
    n->next = next;
    return n;
}

/* Allocates a new node and inserts it in front of given node list. */
static Node *insertNode (IdEntry *entry, Node *lp) {
    return newNode(entry, lp);
}

/* Frees linked-list of nodes. */
static void freeNodes (Node *lp) {
    if (lp == NULL) {
        return;
    }
    freeNodes(lp->next);
    freeIdEntry(lp->entry);
    free(lp);
}

/* Returns pointer to node containing IdEntry identified by identifier and class.
 * If the list does not contain an entry, a NULL pointer is returned.
*/
static Node *listContains (unsigned id, unsigned tc, Node *lp) {
    if (lp == NULL) {
        return NULL;
    }
    if (lp->entry->tc == tc && lp->entry->id == id) {
        return lp;
    }
    return listContains(id, tc, lp->next);
}

/* Debug method: prints linked-list. */
static void printList (Node *lp) {
    if (lp == NULL) {
        printf("[NULL]");
    } else {
        printf("[%s : %s : %s]->", identifierAtIndex(lp->entry->id), tokenClassName(lp->entry->tc), tokenTypeName(lp->entry->tt ));
        printList(lp->next);
    }
}

/*
********************************************************************************
*                         Internal Table Control Routines                      *
********************************************************************************
*/

/* Jenkins hash function: Looted from Meijster's symtab. */
static unsigned hash (const char *s) {
    unsigned i, hash = 0, len = strlen(s);

    for(i = 0; i < len; ++i) {
        hash += s[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash % SYMTAB_SIZE;
}

/* Sets the current table scope level. If invalid, an error is thrown. */
static void setLevel (unsigned level) {
    if (level >= SYMTAB_LVLS) {
        printError("Maximum scope level exceeded!");
        exit(EXIT_FAILURE);
    }
    lvl = level;
}

/* Frees all memory in a given scope level. */
static void freeSymbolTableLevel (unsigned level) {
    if (level >= SYMTAB_LVLS) {
        fprintf(stderr, "Error: freeSymbolTableLevel: Specified scope out of bounds!\n");
        exit(EXIT_FAILURE);
    }

    for (unsigned i = 0; i < SYMTAB_SIZE; i++) {
        freeNodes(symTable[i][level]);
        symTable[i][level] = NULL;
    }
}


/*
********************************************************************************
*                            Table IdEntry Routines                            *
********************************************************************************
*/

/* Copies an IdEntry and returns a pointer. Exits on error. */
IdEntry *copyIdEntry (IdEntry *entry) {
    IdEntry *copy = allocateIdEntry(); 
    copy->id = entry->id;
    copy->tc = entry->tc;
    copy->tt = entry->tt;
    copy->rf = entry->rf;
    copy->data = entry->data;
    return copy;
}


/* Returns pointer to IdEntry if it exists in the symbol table. Else returns
 * NULL. Entries may have the same identifiers so long as their class is unique.
 * 
 * Parameter `scope` defines the search scope. If a positive integer, scope
 *  at literal value of integer is searched. Otherwise all descending table
 *  levels are traversed.
*/ 
IdEntry *containsIdEntry (unsigned id, unsigned tc, int scope) {
    unsigned h = hash(identifierAtIndex(id));
    Node *lp = NULL;

    if (scope == SYMTAB_SCOPE_ALL) {
        for (int l = lvl; l >= 0; l--) {
            if ((lp = listContains(id, tc, symTable[h][l])) != NULL) {
                return lp->entry;
            }
        }
        return NULL;
    } 

    if (scope < SYMTAB_SCOPE_ALL || scope >= SYMTAB_LVLS) {
        fprintf(stderr, "Error: containsIdEntry: Scope out of bounds!\n");
        exit(EXIT_FAILURE);
    }

    if ((lp = listContains(id, tc, symTable[h][scope])) != NULL) {
        return lp->entry;
    }

    return NULL;
}


/* Allocates and installs new IdEntry into the symbol table at current scope.
 * Returns pointer if successful. Duplicate entry must not already exist. 
*/
IdEntry *installIdEntry (unsigned id, unsigned tc, unsigned tt) {
    IdEntry *entry = allocateIdEntry();
    unsigned h = hash(identifierAtIndex(id));
    Node *lp = NULL;

    // Verify entry does not exist yet.
    if ((lp = listContains(id, tc, symTable[h][lvl])) != NULL) {
        fprintf(stderr, "Error: installIdEntry: Entry already exists in table!\n");
        exit(EXIT_FAILURE);
    }

    // Assign entry fields.
    entry->id = id;
    entry->tc = tc;
    entry->tt = tt;
    entry->rf = 0;
    entry->data = (IdData){.argc = 0, .argv = NULL};

    // Insert new entry at list head. Then return pointer to entry.
    symTable[h][lvl] = insertNode(entry, lp);
    return symTable[h][lvl]->entry;
}

/*
********************************************************************************
*                           Table Control Routines                             * 
********************************************************************************
*/

/* Increments table scope level */
void incrementTableScope (void) {
    setLevel(lvl + 1);
}

/* Decrements table scope level: Frees memory in current scope */
void decrementTableScope (void) {
    freeSymbolTableLevel(lvl);
    setLevel(lvl - 1);
}

/* Returns the table scope level */
unsigned currentTableScope (void) {
    return lvl;
}

/* Frees all allocated entires in all table levels */
void freeSymbolTables (void) {
    for (int i = lvl; i >= 0; i--) {
        freeSymbolTableLevel(i);
    }
}

/* Prints all symbol table entires */
void printSymbolTables (void) {
    for (int i = 0; i < SYMTAB_LVLS; i++) {
        printf("****************************** LEVEL %u ******************************\n", i);
        for (int j = 0; j < SYMTAB_SIZE; j++) {
            if (symTable[j][i] != NULL) {
                printf("%d.\t", j); printList(symTable[j][i]); putchar('\n');
            }
        }
    }
}
