#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#ifndef SHELL_COMMAND_H
#define SHELL_COMMAND_H

typedef struct Command
{
    char **args;
    char *input;
    char *output;
    uint32_t argCount;
    bool isAppend;
} Command;

typedef struct Link
{
    struct Link *next;
    Command c;

} Link;

typedef struct List
{
    Link *first, *last;
} List;

List initList();
int addNewCommand(List *l);
int addNewArg(List *l);
int setArgStart(List *l, char *start);
int setInputStart(List *l, char *start);
int setOutputStart(List *l, char *start);
int setIsAppend(List *l, bool isAppend);
int clearList(List *l);
int printCommand(Command *c);
int printList(List *l);

#endif