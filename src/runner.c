#include "runner.h"
#include "errno.h"

int runFork(
    int (*child)(void **),
    void **childArgs,
    int (*parent)(void **),
    void **parentArgs)
{
    int forkRes = 0;
    ASSERT((forkRes = fork()) < 0, return -1);

    if (forkRes == 0)
    {
        int childRes = 0;
        ASSERT((childRes = child(childArgs)) != 0, _exit(1));
        _exit(0);
    }
    else
    {
        int success = 0;
        if (parent)
            ASSERT((success = parent(parentArgs)) < 0, );

        int status = 0, exitedChild = 0;
        ASSERT((exitedChild = wait(&status)) < 0, success = -1);

        if (!WIFEXITED(status) || success < 0)
            return -1;
        return -WEXITSTATUS(status);
    }
    return 0;
}

int runForkF(void **args)
{
    ASSERT(runFork(
               (int (*)(void **))args[0],
               (void **)args[1],
               (int (*)(void **))args[2],
               (void **)args[3]) < 0,
           return -1);
    return 0;
}

static int execvpF(void **args)
{
    char *file = (char *)args[0];
    char **argv = (char **)args[1];
    ASSERT(execvp(file, argv) != 0, return 1);
    return 0;
}

static int setInputTo(int fd)
{
    if (fd == STDIN_FILENO)
        return 0;
    ASSERT(dup2(fd, STDIN_FILENO) < 0, return -1);
    ASSERT(close(fd) < 0, return -1);
    return 0;
}

static int setOutputTo(int fd)
{
    if (fd == STDOUT_FILENO)
        return 0;
    ASSERT(dup2(fd, STDOUT_FILENO) < 0, return -1);
    ASSERT(close(fd) < 0, return -1);
    return 0;
}

static int copyIO(int *originalIO)
{
    ASSERT((originalIO[0] = dup(STDIN_FILENO)) < 0, return -1);
    ASSERT((originalIO[1] = dup(STDOUT_FILENO)) < 0, return -1);
    return 0;
}

static int restoreInput(int *originalIO)
{
    ASSERT(dup2(originalIO[0], STDIN_FILENO) < 0, return -1);
    ASSERT(close(originalIO[0]) < 0, return -1);
    return 0;
}

static int restoreOutput(int *originalIO)
{
    ASSERT(dup2(originalIO[1], STDOUT_FILENO) < 0, return -1);
    ASSERT(close(originalIO[1]) < 0, return -1);
    return 0;
}

static void emptySigIntHandler(int s) {}

int runCommand(Command *c)
{
    bool success = true;
    __sighandler_t prev;
    ASSERT((prev = signal(SIGINT, emptySigIntHandler)) == SIG_ERR, return 1);
    void *args[] = {c->args[0], &c->args[0]};
    if (runFork(execvpF, args, NULL, NULL) < 0)
        success = false;
    ASSERT(signal(SIGINT, prev) == SIG_ERR, return 1);

    if (!success)
    {
        printCommand(c);
        fprintf(stderr, "\tX\n");
    }

    return 0;
}

static int runPipe(List *l)
{
    // fprintf(stderr, "Plan to run:\n");
    // printList(l);
    // fprintf(stderr, "================================\n\n");

    if (l->first->next == NULL)
    {
        ASSERT(runCommand(&l->first->c) < 0, return -1);
    }
    else
    {
        int originalIO[2] = {0};
        ASSERT(copyIO(originalIO) < 0, return -1);

        int pipeFds[2] = {0, 0};
        ASSERT(pipe(pipeFds) < 0, return -1);
        ASSERT(setOutputTo(pipeFds[1]) < 0, return -1);
        ASSERT(runCommand(&l->first->c) < 0, return -1);

        Link *t = l->first->next;
        while (t->next != NULL)
        {
            Link *temp = t->next;
            ASSERT(setInputTo(pipeFds[0]) < 0, return -1);
            ASSERT(pipe(pipeFds) < 0, return -1);
            ASSERT(setOutputTo(pipeFds[1]) < 0, return -1);
            ASSERT(runCommand(&t->c) < 0, return -1);
            t = temp;
        }

        ASSERT(setInputTo(pipeFds[0]) < 0, return -1);
        ASSERT(restoreOutput(originalIO) < 0, return -1);
        ASSERT(runCommand(&l->last->c) < 0, return -1);
        ASSERT(restoreInput(originalIO) < 0, return -1);
    }
    // fprintf(stderr, "========Run successful=========\n\n");
    return 0;
}

static int openAppend(const char *file)
{
    errno = 0;
    int fd = open(file, O_WRONLY | O_CREAT | O_EXCL, 0644);
    if (fd < 0)
    {
        ASSERT(errno != EEXIST, return -1);
        errno = 0;
        ASSERT((fd = open(file, O_WRONLY | O_APPEND)) < 0, return -1);
    }
    return fd;
}

int runList(List *l)
{
    ASSERT(l->first == NULL, return -1);
    int originalIO[2] = {0, 0};
    ASSERT(copyIO(originalIO) < 0, return -1);
    bool hadCustomInput = false, hadCustomOutput = false;

    Command *first = &l->first->c;
    Command *last = &l->last->c;

    if (first->input)
    {
        hadCustomInput = true;
        int fd = 0;
        ASSERT((fd = open(first->input, O_RDONLY)) < 0, return -1);
        ASSERT(setInputTo(fd) < 0, return -1);
    }

    if (last->output)
    {
        hadCustomOutput = true;
        int fd = 0;
        if (last->isAppend)
        {
            ASSERT((fd = openAppend(last->output)) < 0, return -1);
        }
        else
        {
            ASSERT((fd = open(last->output, O_WRONLY | O_CREAT, 0644)) < 0, return -1);
        }
        ASSERT(setOutputTo(fd) < 0, return -1);
    }

    ASSERT(runPipe(l) < 0, return -1);

    if (hadCustomInput)
    {
        ASSERT(restoreInput(originalIO) < 0, return -1);
    }
    if (hadCustomOutput)
    {
        ASSERT(restoreOutput(originalIO) < 0, return -1);
    }
    return 0;
}

int require(List *l)
{
    // fprintf(stderr, "Require ");
    ASSERT(runList(l) < 0, return -1);
    return 0;
}