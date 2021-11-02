#include "command.h"
#include "assert.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef SHELL_RUNNER_H
#define SHELL_RUNNER_H

int runCommand(Command *c);
int runList(List *l);
int require(List *l);
int runFork(
    int (*child)(void **),
    void **childArgs,
    int (*parent)(void **),
    void **parentArgs);

int runForkF(void **args);

#endif