#include "assert.h"
#include "command.h"
#include "interpreter.h"
#include "shell.h"

extern int allocated, deallocated;

int main()
{
    printf("Welcome to my_shell!\n");
    ASSERT(runShell() < 0, return -1);
    printf("Allocated: %d, deallocated: %d\nGood bye!\n", allocated, deallocated);

    //cat < ../ls.txt | grep a -i| cat >> ../ls.txt
    // const char *s = "cat < ../ls.txt | grep a -i| cat >> ../ls.txt";
    // uint32_t length = strlen(s);
    // char text[1024] = {0};
    // strcpy(text, s);

    // ASSERT(interpret(text, length) < 0, return -1);

    return 0;
}