#include "shell.h"
#include <signal.h>

static int readStr(char **str, uint32_t *length)
{
    uint32_t allocSize = 15, curSize = 0, nexAlloc = 10;
    char *s = NULL;
    ASSERT((s = (char *)malloc(allocSize * sizeof(char))) == NULL, return -1);

    bool hadEof = false;

    while (true)
    {
        int c = getchar();

        if (c == '\n')
            break;

        s[curSize++] = c;

        if (curSize == allocSize)
        {
            char *newStr = NULL;
            uint32_t newSize = allocSize + nexAlloc;
            ASSERT((newStr = (char *)malloc(newSize * sizeof(char))) == NULL, return -1);
            allocSize = newSize;
            memcpy(newStr, s, allocSize);
            free(s);
            s = newStr;
        }
    }
    s[curSize] = '\0';
    *str = s;
    *length = curSize;

    if (hadEof)
        return 1;

    return 0;
}

static bool isRunning = true;

static void shellCloseHandler(int s)
{
    isRunning = false;
}

static int interpretF(void **args)
{
    ASSERT(interpret((char *)args[0], *(int *)args[1]) < 0, return -1);
    printf("%s Ended successfully!\n", (char *)args[0]);
    ASSERT(kill(getppid(), SIGUSR1) < 0, return -1);
    return 0;
}

static int runShellF(void **args)
{
    ASSERT(runShell() < 0, return -1);
    return 0;
}

int runShell()
{
    // printf("PID %d new shell\n", getpid());
    ASSERT(signal(SIGUSR1, shellCloseHandler) < 0, return -1);
    ASSERT(signal(SIGINT, shellCloseHandler) < 0, return -1);
    char *s = NULL;
    uint32_t length = 0;

    while (isRunning)
    {
        int readRes = 0;
        printf("> ");
        ASSERT((readRes = readStr(&s, &length)) < 0, return -1);
        if (readRes == 1)
            isRunning = false;

        if (!isCorrect(s, length))
        {
            fprintf(stderr, "Invalid input\n");
        }
        else if (getIsParallel(s, length))
        {
            printf("Running parallel\n");
            void *interpreterArgs[] = {s, &length};
            void *forkArgs[] = {interpretF, interpreterArgs, runShellF, NULL};
            ASSERT(runFork(runForkF, forkArgs, NULL, NULL) < 0, return -1);
        }
        else if (interpret(s, length) < 0)
        {
            fprintf(stderr, "Invalid input\n");
        }
        free(s);
    }

    // printf("PID %d leaving shell\n", getpid());
    return 0;
}