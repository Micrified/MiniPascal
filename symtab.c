#include "symtab.h"

/*
********************************************************************************
*                           Internal Data Structures                           *
********************************************************************************
*/

typedef struct node {
    IdEntry entry;
    struct node *next;
} Node;

/*
********************************************************************************
*                      Symbolic Constants & Global Variables                   *
********************************************************************************
*/

#define SYMTAB_SIZE         6959
#define SYMTAB_LEVELS       2

// Symbol table.
Node *symTable[SYMTAB_SIZE][SYMTAB_LEVELS];

// Current scope level of the table.
unsigned level;

/*
********************************************************************************
*                               IdEntry Routines                               *
********************************************************************************
*/

/* Returns nonzero if a is the same entry as b */
int equalsID (IdEntry a, IdEntry b) {
    return (strcmp(identifierAtIndex(a.id), identifierAtIndex(b.id)) == 0 &&
            a.tt == b.tt);
}

/* Convenient initializer for IdEntry. */
IdEntry newIDEntry (unsigned id, unsigned tt) {
    return (IdEntry){.id = id, .tt = tt};
}

/*
********************************************************************************
*                           Private Linked List Routines                       *
********************************************************************************
*/

/* Safely allocates a new node */
static Node *newNode (IdEntry entry, Node *next) {
    Node *n = NULL;
    if ((n = malloc(sizeof(Node))) == NULL) {
        fprintf(stderr, "Error: symtab: Couldn't init a new Node!\n");
        exit(EXIT_FAILURE);
    }
    n->entry = entry;
    n->next = next;
    return n;
}

/* Allocates a new node and inserts it in front of the list */
static Node *insertNode (IdEntry entry, Node *lp) {
    return newNode(entry, lp);
}

/* Free's a linked list of nodes */
static void freeNode (Node *lp) {
    if (lp == NULL) {
        return;
    }
    freeNode(lp->next);
    free(lp);
}

/* Returns pointer to node if the IdEntry exists in the list. Else NULL */
static Node *listContains (const char *identifier, Node *lp) {
    if (lp == NULL) {
        return 0;
    }
    if (strcmp(identifier, identifierAtIndex(lp->entry.id)) == 0) {
        return lp;
    }
    return listContains(identifier, lp->next);
}

/* Debug Method: Prints a linked node list */
static void printList (Node *lp) {
    if (lp == NULL) {
        printf("[Null]");
    } else {
        printf("[%s]->", identifierAtIndex(lp->entry.id));
        printList(lp->next);
    }
}

/*
********************************************************************************
*                              Private Table Routines                          *
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


/*
********************************************************************************
*                               Public Table Routines                          *
********************************************************************************
*/



/* Sets the current table level/scope */
void setLevel (unsigned lvl) {
    if (lvl > SYMTAB_LEVELS) {
        fprintf(stderr, "Error: Bad Level: (%d > %d)\n", lvl, SYMTAB_LEVELS);
        exit(EXIT_FAILURE);
    }
    level = lvl;
}


/* Returns pointer to IdEntry if in table. Else NULL */
IdEntry *tableContains (const char *identifier) {
    unsigned k = hash(identifier);
    Node *lp = listContains(identifier, symTable[k][level]);

    return (lp == NULL) ? NULL : &(lp->entry);
}

/* Inserts a new IdEntry into the table. Returns pointer to entry. */
IdEntry *installEntry (IdEntry entry) {
    const char *identifier = identifierAtIndex(entry.id);
    unsigned k = hash(identifier);
    Node *lp = NULL;

    // If the entry exists, then produce an error (should have looked up).
    if ((lp = listContains(identifier, symTable[k][level])) != NULL) {
        fprintf(stderr, "Error: Try to avoid installing something twice!\n");
        exit(EXIT_FAILURE);
    }

    // Insert new entry at list head, return pointer to entry.
    symTable[k][level] = insertNode(entry, symTable[k][level]);
    return &(symTable[k][level]->entry);
}

/* Removes all entries in the given level */
void freeTableLevel (int lvl) {
    for (int i = 0; i < SYMTAB_SIZE; i++) {
        freeNode(symTable[i][lvl]);
        symTable[i][lvl] = NULL;
    }
}

/* Debug Method: Print state of the symbol tables */
void printSymbolTables (void) {
    for (int i = 0; i < SYMTAB_LEVELS; i++) {
        printf("****************************** LEVEL %u ******************************\n", i);
        for (int j = 0; j < SYMTAB_SIZE; j++) {
            printf("%d.\t", j); printList(symTable[j][i]); putchar('\n');
        }
     }
}


