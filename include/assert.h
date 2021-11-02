#ifndef SHELL_ASSERT_H
#define SHELL_ASSERT_H

//spellchecker: ignore stringize

#define STRINGIZE2(x) #x
#define STRINGIZE(x) STRINGIZE2(x)

#define TRACE() \
    __FILE__    \
    ":" STRINGIZE(__LINE__)

#define ASSERT(cond, err)                 \
    {                                     \
        if (cond)                         \
        {                                 \
            perror(#cond " at " TRACE()); \
            err;                          \
        }                                 \
    }

#endif