#ifndef VARINT_H
#define VARINT_H

#include <stddef.h>
#include <stdint.h>

/**
 * Read a variable-length integer from the start of a buffer
 *
 * @remarks A varint can be between 1 and 9 bytes, the buffer is only expected to be as long as
 * is needed for the contained value
 *
 * @param buf Buffer to read from
 * @param [out] value Value of the varint
 * @return Number of bytes read, or 0 if there was an error
 */
size_t read_varint(const uint8_t *buf, uint64_t *value);

/**
 * Write an integer into the buffer as a variable-length integer
 *
 * @remarks The buffer should assume the value will take the maximum length of 9 bytes and
 * have the requisite capacity
 *
 * @param buf Buffer to write to
 * @param value Value to write
 * @return Encoded length of the value, in bytes
 */
size_t write_varint(uint8_t *buf, const uint64_t value);

#endif
