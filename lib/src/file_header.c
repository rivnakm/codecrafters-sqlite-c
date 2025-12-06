#include "cc_sqlite/file_header.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#define assert_fpos(file, pos) assert(ftell(file) == pos)

int file_header_read(FILE *file, FileHeader *header)
{
    int err = fseek(file, 0x10, SEEK_SET); // First 16 bytes is the header "SQLite format 3" + '\0'
    if (err != 0)
    {
        return err;
    }

    uint8_t buffer[4];
    assert_fpos(file, 0x10L);
    fread(buffer, sizeof(uint8_t), 2, file);
    assert_fpos(file, 0x12L);

    header->page_size = (buffer[0] << 8) | buffer[1];

    fseek(file, 1, SEEK_CUR);
    // TODO: file format write version

    fseek(file, 1, SEEK_CUR);
    // TODO: file format read version

    assert_fpos(file, 0x14L);
    fread(buffer, sizeof(uint8_t), 1, file);
    header->reserved = buffer[0];

    fseek(file, 48, SEEK_SET);
    assert_fpos(file, 0x30L);
    fread(buffer, sizeof(uint8_t), 4, file);
    header->suggested_cache_size = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];

    return EXIT_SUCCESS;
}
