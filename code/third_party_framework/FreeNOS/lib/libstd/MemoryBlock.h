// Copyright (C) 2018 qmsggg

#ifndef MEMORYBLOCK_H
#define MEMORYBLOCK_H
 
#include "Types.h"

class MemoryBlock
{
  public:
    
    /**
     * Fill memory with a constant byte
     * @param dest Memory to write to.
     * @param ch Constant byte.
     * @return Ponter to dest.
     */
    static void * set(void *dest, int ch, unsigned count);
    
    /**
     * Copy memory from one place to another.
     * @param dest Destination addres.
     * @param src Source address.
     * @param count Number of bytes to copy
     * @return Number of bytes copied.
     */
    static Size copy(void *dest, const void *src, Size count);

    /**
     * Copy a character string.
     */
    static Size copy(char *dest, char *src, Size count);

    /**
     * Compare memory.
     */
    static bool compare(void *dest, void *src, Size count);

    /**
     * Compare memory.
     *
     * @param p1 Memory pointer one.
     * @param p2 Memory pointer two.
     * @param count Number of bytes to compare or zero to continue until a ZERO byte.
     * @return True if equal, false otherwise.
     */
    static bool compare(const char *p1, const char *p2, Size count = 0);
};
 
#endif /* MEMORYBLOCK_H */
