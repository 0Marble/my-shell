#include "command.h"
#include "assert.h"
#include "runner.h"

#ifndef SHELL_INTERPRETER_H
#define SHELL_INTERPRETER_H

int interpret(char *s, uint32_t length);
bool isCorrect(char *s, uint32_t length);
bool getIsParallel(char *s, uint32_t length);

#endif