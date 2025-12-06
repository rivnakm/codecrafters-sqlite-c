#include "cc_sqlite/alloc.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "exit_codes.h"

#ifdef ARENA_ALLOC_DEBUG
#include <stdio.h>
#endif

Arena *arena_create(ptrdiff_t size)
{
    Arena *arena = malloc(sizeof(Arena));
    if (!arena)
    {
        exit(EXIT_ALLOCATION_ERROR);
    }

    arena->start = malloc(size);
    if (!arena->start)
    {
        free(arena);
        exit(EXIT_ALLOCATION_ERROR);
    }

    arena->end = arena->start + size;
    arena->current = arena->start;

    return arena;
}

void arena_destroy(Arena *arena)
{
    free(arena->start);
    free(arena);
}

void *arena_alloc(Arena *const arena, ptrdiff_t size, ptrdiff_t alignment)
{
    ptrdiff_t padding = -(uintptr_t)arena->current & (alignment - 1);
    ptrdiff_t available = arena->end - arena->current - padding;

    if (available < 0 || size > available)
    {
        exit(EXIT_ALLOCATION_ERROR);
    }

    void *p = arena->current + padding;
    arena->current += size + padding;

#ifdef ARENA_ALLOC_DEBUG
    fprintf(stderr, "Allocated %ld bytes at %p\n", size, p);
    fprintf(stderr, "Arena has %ld of %ld bytes allocated (%ld bytes remaining)\n", arena->current - arena->start,
            arena->end - arena->start, arena->end - arena->current);
#endif
    return memset(p, 0, size);
}

char *arena_strdup(Arena *const arena, const char *str)
{
    size_t len = strlen(str) + 1;
    char *p = arena_alloc(arena, len, _Alignof(char));

    return memcpy(p, str, len);
}
