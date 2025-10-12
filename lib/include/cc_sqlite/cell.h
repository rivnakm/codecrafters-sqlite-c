#ifndef CCSQLITE_CELL_H
#define CCSQLITE_CELL_H

#include <stdint.h>
#include <stdio.h>

#include "cc_sqlite/page_header.h"

/**
 * Read the contents of a cell payload
 *
 * @param file Database file to read
 * @param cell_pointer Cell start position
 * @param page_type Page type
 * @param usable_page_size Usable page size, page_size less reserved space
 * @param data [out] Cell payload
 * @param size Payload length
 * @return Status code
 */
int read_cell_payload(FILE *file, const uint16_t cell_pointer, const PageType page_type,
                      const uint16_t usable_page_size, uint8_t **data, size_t *size);

#endif
