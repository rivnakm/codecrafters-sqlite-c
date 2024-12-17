#ifndef BYTES_H
#define BYTES_H

#include <stddef.h>
#include <stdint.h>

/**
 * Write an unsigned integer of specified length to the buffer as big-endian bytes
 *
 * @param dest Buffer to write to
 * @param value Value to write
 * @param n Length of the integer in bytes
 */
void to_be_bytes(uint8_t *dest, const uint64_t value, const size_t n);

/**
 * Write a signed integer of specified length to the buffer as big-endian, two's complement bytes
 *
 * @param dest Buffer to write to
 * @param value Value to write
 * @param n Length of the integer in bytes
 */
void to_signed_be_bytes(uint8_t *dest, const int64_t value, const size_t n);

/**
 * Reads a specified number of big-endian bytes from a buffer as an unsigned integer
 *
 * @param src Buffer to read from
 * @param n Number of bytes to read
 * @return Value of the sequence of bytes
 */
uint64_t from_be_bytes(const uint8_t *src, const size_t n);

/**
 * Reads a specified number of big-endian, two's complement bytes from a buffer as a signed integer
 *
 * @param src Buffer to read from
 * @param n Number of bytes to read
 * @return Value of the sequence of bytes
 */
int64_t from_signed_be_bytes(const uint8_t *src, const size_t n);

#endif
