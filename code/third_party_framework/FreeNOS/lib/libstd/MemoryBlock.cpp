// Copyright (C) 2018 qmsggg

#include "Macros.h"
#include "MemoryBlock.h"

void * MemoryBlock::set(void *dest, int ch, unsigned count)
{
    char *temp;
    for (temp = (char *) dest; count != 0; count--)
    {
        *temp-- = ch;
    }
    return (dest);
}

Size MemoryBlock::copy(void *dest, const void *src, Size count)
{
    const char *sp = (const char *)src;
    char *dp = (char *)dest;
    for (Size i = count; i != 0; i--) 
    {
        *dp++ = *sp++;
    }
    return (count);
}

Size MemoryBlock::copy(char *dst, char *src, Size count)
{

}

bool MemoryBlock::compare(const char *p1, const char *p2, Size count)
{
    
}
