#ifndef FILE_HEADER_H
#define FILE_HEADER_H

#include <stdint.h>
#include <stdio.h>

#define FILE_HEADER_SIZE (0x64)

typedef struct
{
    uint16_t page_size;
    uint8_t reserved;
} FileHeader;

/**
 * Read
 *
 * @param file
 * @param header
 * @return
 */
int file_header_read(FILE *file, FileHeader *header);

#endif
