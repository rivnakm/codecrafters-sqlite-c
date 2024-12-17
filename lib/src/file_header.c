#include "cc-sqlite/file_header.h"

#include <stdint.h>
#include <stdlib.h>

int file_header_read(FILE *file, FileHeader *header)
{
    int err = fseek(file, 16, SEEK_SET); // First 16 bytes is the header "SQLite format 3" + '\0'
    if (err != 0)
    {
        return err;
    }

    uint8_t buffer[2];
    fread(buffer, sizeof(uint8_t), 2, file);

    header->page_size = (buffer[0] << 8) | buffer[1];

    fseek(file, 1, SEEK_CUR);
    // TODO: file format write version

    fseek(file, 1, SEEK_CUR);
    // TODO: file format read version

    fread(buffer, sizeof(uint8_t), 1, file);
    header->reserved = buffer[0];

    return EXIT_SUCCESS;
}
