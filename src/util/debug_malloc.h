#ifndef TANKS_DEBUG_MALLOC_H
#define TANKS_DEBUG_MALLOC_H

#include <cstdint>
#include <cstdlib>
#include <debug.h>

#if 1

inline void *malloc_(size_t size, const char* file, uint24_t line) {
    void *ptr = malloc(size);
    dbg_printf("ALLOC: %u @ %p (%s:%u)\n", size, ptr, file, line);
    return ptr;
}

inline void free_(void *ptr, const char* file, uint24_t line) {
    dbg_printf("FREE: %p (%s:%u)\n", ptr, file, line);
    free(ptr);
}

#define malloc(size) malloc_(size, __FILE_NAME__, __LINE__)
#define free(ptr) free_(ptr, __FILE_NAME__, __LINE__)

void* operator new (std::size_t size) {
    if (size == 0)
        size = 1;
    void* ptr = malloc(size);
    return ptr;
}

#endif

#endif //TANKS_DEBUG_MALLOC_H
