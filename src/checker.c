#include "interpreter.h"
#include <ctype.h>

/*
    A-start, B-conveyor, C-command with input, D-conv without input, E-conv without input and output, F-command with output, G-command with no in and no out. a-argument
    A -> B&&A , B& , B
    B -> C|D , D , C
    D -> E|F , E, F
    E -> G|E , G
    C -> G<a
    F -> G>a , G>>a
    G -> a G , a
*/

static bool isA(const char *s, uint32_t from, uint32_t to);
static bool isB(const char *s, uint32_t from, uint32_t to);
static bool isC(const char *s, uint32_t from, uint32_t to);
static bool isD(const char *s, uint32_t from, uint32_t to);
static bool isE(const char *s, uint32_t from, uint32_t to);
static bool isF(const char *s, uint32_t from, uint32_t to);
static bool isG(const char *s, uint32_t from, uint32_t to);
static bool isa(const char *s, uint32_t from, uint32_t to);

static bool isa(const char *s, uint32_t from, uint32_t to)
{
    const char *r = s + from;

    if (from > to)
        return false;

    bool hadWord = false, hadPostSpace = false;
    for (uint32_t i = from; i <= to; i++)
    {
        if (s[i] == '<' || s[i] == '>' || s[i] == '|')
            return false;

        if (!isspace(s[i]))
            hadWord = true;
        if (isspace(s[i]) && hadWord)
            hadPostSpace = true;
        if (!isspace(s[i]) && hadPostSpace)
            return false;
    }
    return true;
}

static bool isG(const char *s, uint32_t from, uint32_t to)
{
    const char *r = s + from;

    if (from > to)
        return false;

    for (uint32_t i = from; i <= to; i++)
    {
        if (s[i] == '<' || s[i] == '>' || s[i] == '|')
            return false;

        if (isspace(s[i]))
            if (i > from && isa(s, from, i - 1) && isG(s, i + 1, to))
                return true;
    }
    bool res = isa(s, from, to);
    return res;
}

static bool isF(const char *s, uint32_t from, uint32_t to)
{
    const char *r = s + from;

    if (from > to)
        return false;
    bool hadRarr = false, res = false;
    for (uint32_t i = from; i <= to; i++)
    {
        if (s[i] == '<' || s[i] == '|')
            return false;
        if (s[i] == '>')
        {
            if (i < to && s[i + 1] == '>')
                res = i > from && isG(s, from, i - 1) && isa(s, i + 2, to);
            else
                res = i > from && isG(s, from, i - 1) && isa(s, i + 1, to);
            return res;
        }
    }

    return false;
}

static bool isC(const char *s, uint32_t from, uint32_t to)
{
    const char *r = s + from;
    bool res = false;
    if (from > to)
        return false;

    for (uint32_t i = from; i <= to; i++)
    {
        if (s[i] == '>' || s[i] == '|')
            return false;
        if (s[i] == '<')
        {
            res = i > from && isG(s, from, i - 1) && isa(s, i + 1, to);
            return res;
        }
    }
    return false;
}

static bool isE(const char *s, uint32_t from, uint32_t to)
{
    const char *r = s + from;

    if (from > to)
        return false;

    bool res = false;
    for (uint32_t i = from; i <= to; i++)
    {
        if (s[i] == '|')
        {
            res = i > from && isG(s, from, i - 1) && isE(s, i + 1, to);
            return res;
        }
    }
    res = isG(s, from, to);
    return res;
}

static bool isD(const char *s, uint32_t from, uint32_t to)
{
    const char *r = s + from;

    if (from > to)
        return false;
    bool res = false;
    for (uint32_t i = from; i <= to; i++)
    {
        if (s[i] == '|')
        {
            if (i > from && isE(s, from, i - 1) && isF(s, i + 1, to))
                return true;
        }
    }
    res = isE(s, from, to) || isF(s, from, to);
    return res;
}

static bool isB(const char *s, uint32_t from, uint32_t to)
{
    const char *r = s + from;

    if (from > to)
        return false;
    bool res = false;
    for (uint32_t i = from; i <= to; i++)
    {
        if (s[i] == '|')
        {
            if (i > from && isC(s, from, i - 1) && isD(s, i + 1, to))
                return true;
        }
    }
    res = isD(s, from, to) || isC(s, from, to);
    return res;
}

static bool isA(const char *s, uint32_t from, uint32_t to)
{
    const char *r = s + from;

    if (from > to)
        return false;
    bool res = false;
    for (uint32_t i = from; i <= to; i++)
    {
        if (s[i] == '&')
        {
            if (i < to && s[i + 1] == '&')
            {
                res = i > from && isB(s, from, i - 1) && isA(s, i + 2, to);
                return res;
            }
            for (uint32_t j = i + 1; j <= to; j++)
                if (!isspace(s[j]))
                    return false;
            res = i > from && isB(s, from, i - 1);
            return res;
        }
    }
    res = isB(s, from, to);
    return res;
}

bool isCorrect(char *s, uint32_t length)
{
    if (length == 0)
        return false;
    return isA(s, 0, length);
}
