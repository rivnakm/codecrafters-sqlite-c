#ifndef CC_SQLITE_ALLOC_H
#define CC_SQLITE_ALLOC_H

#include <stddef.h>

typedef struct
{
    char *start;
    char *current;
    char *end;
} Arena;

Arena *arena_create(ptrdiff_t size);
void arena_destroy(Arena *arena);
void *arena_alloc(Arena *const arena, ptrdiff_t size, ptrdiff_t alignment);
char *arena_strdup(Arena *const arena, const char *str);

#endif
