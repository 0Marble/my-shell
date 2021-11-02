#include "command.h"
#include "assert.h"

int allocated = 0, deallocated = 0;

static void *mallocLog(size_t size)
{
    allocated++;
    void *ptr = malloc(size);
    // printf("Allocated %p\n", ptr);
    return ptr;
}

static void freeLog(void *ptr)
{
    if (ptr == NULL)
        return;
    // printf("Deallocated %p\n", ptr);
    deallocated++;
    free(ptr);
}

//expands array of curSize bytes by expandBy bytes and returns new size in curSize
static int expandArray(void **array, uint32_t *curSize, uint32_t expandBy)
{
    uint32_t newSize = *curSize + expandBy;
    void *newArray = NULL;
    ASSERT((newArray = mallocLog(newSize)) == NULL, return -1);

    if (*curSize != 0)
    {
        memcpy(newArray, *array, *curSize);
        freeLog(*array);
    }
    *array = newArray;
    *curSize = newSize;

    return 0;
}
static Command initCommand()
{
    Command c;
    c.args = NULL;
    c.input = NULL;
    c.output = NULL;
    c.argCount = 0;
    c.isAppend = false;
    return c;
}

static Link *makeLink()
{
    Link *l = NULL;
    ASSERT((l = (Link *)mallocLog(sizeof(Link))) == NULL, return NULL);
    l->next = NULL;
    l->c = initCommand();
    return l;
}

List initList()
{
    List l;
    l.first = l.last = NULL;
    return l;
}

int addNewCommand(List *l)
{
    Link *n = NULL;
    ASSERT((n = makeLink()) == NULL, return -1);
    ASSERT((n->c.args = (char **)mallocLog(sizeof(char *))) == NULL, return -1);
    n->c.argCount = 1;

    if (l->last == NULL)
        l->first = n;
    else
        l->last->next = n;
    l->last = n;
    return 0;
}

int addNewArg(List *l)
{
    ASSERT(l->last == NULL, return -1);
    char ***args = &l->last->c.args;
    uint32_t curSize = l->last->c.argCount * sizeof(char *);
    ASSERT(expandArray((void **)args, &curSize, sizeof(char *)) < 0, return -1);
    (*args)[l->last->c.argCount] = NULL;
    l->last->c.argCount++;

    return 0;
}

int setArgStart(List *l, char *start)
{
    ASSERT(l->last == NULL, return -1);
    ASSERT(l->last->c.argCount == 0, return -1);

    l->last->c.args[l->last->c.argCount - 2] = start;
    return 0;
}

int setInputStart(List *l, char *start)
{
    ASSERT(l->last == NULL, return -1);
    l->last->c.input = start;
    return 0;
}

int setOutputStart(List *l, char *start)
{
    ASSERT(l->last == NULL, return -1);
    l->last->c.output = start;
    return 0;
}

int setIsAppend(List *l, bool isAppend)
{
    ASSERT(l->last == NULL, return -1);
    l->last->c.isAppend = isAppend;
    return 0;
}

int printCommand(Command *c)
{
    fprintf(stderr, "%s -> ", c->input);

    for (uint32_t i = 0; i < c->argCount; i++)
        fprintf(stderr, "%s_", c->args[i]);
    fprintf(stderr, " -> %s", c->output);
    return 0;
}

int printList(List *l)
{
    Link *t = l->first;
    while (t != NULL)
    {
        Link *temp = t->next;
        printCommand(&t->c);
        fprintf(stderr, "\n");
        t = temp;
    }
    return 0;
}

int clearList(List *l)
{
    Link *t = l->first;
    while (t != NULL)
    {
        Link *temp = t->next;
        if (t->c.argCount > 0)
            freeLog(t->c.args);
        freeLog(t);
        t = temp;
    }
    l->last = l->first = NULL;
    return 0;
}