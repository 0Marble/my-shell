#include "interpreter.h"
#include <ctype.h>

bool getIsParallel(char *s, uint32_t length)
{
    bool isParallel = false;

    for (uint32_t i = length;; i--)
    {
        if (!isspace(s[i]) && s[i] != '\0')
        {
            if (s[i] == '&')
            {
                isParallel = true;
                break;
            }
            break;
        }
        if (i == 0)
            break;
    }
    return isParallel;
}

int interpret(char *s, uint32_t length)
{
    bool wasSpace = true, wasPipe = true;
    bool wasAnd = false;
    bool wasLarr = false, wasRarr = false;
    bool inCommand = false;

    List commands = initList();

    for (uint32_t i = 0; i < length; i++)
    {
        char c = s[i];

        if (c == '&')
        {
            if (wasAnd)
            {
                if (require(&commands) < 0)
                    return -1;
                wasAnd = false;

                ASSERT(clearList(&commands) < 0, return -1);
                wasSpace = wasPipe = true;
            }
            else
                wasAnd = true;
        }

        if (isspace(c))
            wasSpace = true;
        if (c == '|')
            wasSpace = wasPipe = true;

        if (c == '<')
            wasLarr = true;
        if (c == '>')
        {
            if (wasRarr)
            {
                ASSERT(setIsAppend(&commands, true) < 0, return -1);
            }
            wasRarr = true;
        }

        if (wasPipe && !isspace(c) && c != '|' && c != '&')
        {
            ASSERT(addNewCommand(&commands) < 0, return -1);
            wasRarr = wasLarr = false;
        }
        if (wasSpace && !isspace(c) && !wasLarr && !wasRarr && c != '|' && c != '&')
        {
            ASSERT(addNewArg(&commands) < 0, return -1);
        }

        if (c == '|')
            wasSpace = wasPipe = true;
        if (!isspace(c) && c != '<' && c != '>' && c != '|' && c != '&')
        {
            wasSpace = wasPipe = false;
            if (!inCommand)
            {
                if (wasLarr)
                {
                    ASSERT(setInputStart(&commands, s + i) < 0, return -1);
                    inCommand = true;
                }
                else if (wasRarr)
                {
                    ASSERT(setOutputStart(&commands, s + i) < 0, return -1);
                    inCommand = true;
                }
                else
                {
                    ASSERT(setArgStart(&commands, s + i) < 0, return -1);
                    inCommand = true;
                }
            }
        }
        else
        {
            s[i] = '\0';
            inCommand = false;
        }
    }

    ASSERT(runList(&commands) < 0, return -1);
    ASSERT(clearList(&commands) < 0, return -1);

    return 0;
}