#include "cc-sqlite/varint.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "cc-sqlite/bytes.h"

#define VARINT_MAX_BYTES (9)

size_t read_varint(const uint8_t *buf, int64_t *value)
{
    size_t len = 0;
    uint64_t temp = 0;
    for (size_t i = 0; i < VARINT_MAX_BYTES; i++)
    {
        len++;

        uint8_t current = buf[i];
        if (i < VARINT_MAX_BYTES - 1)
        {
            current &= 0x7F;
            temp <<= 7;
        }
        else
        {
            temp <<= 8;
        }
        temp |= current;

        if ((buf[i] & 0x80) == 0)
        {
            break;
        }
    }

    uint8_t twos_comp[INT64_WIDTH] = {0};
    to_be_bytes(twos_comp, temp, INT64_WIDTH);
    *value = from_signed_be_bytes(twos_comp, INT64_WIDTH);

    return len;
}

size_t write_varint(uint8_t *buf, const int64_t value)
{
    if (value == 0)
    {
        buf[0] = 0;
        return 1;
    }

    uint8_t twos_comp[INT64_WIDTH] = {0};
    to_signed_be_bytes(twos_comp, value, INT64_WIDTH);

    uint64_t temp = from_be_bytes(twos_comp, INT64_WIDTH);

    size_t digits = 0;
    for (uint64_t v = temp; v > 0; v >>= 1)
    {
        digits++;
    }

    size_t len = 0;
    if (digits > 56)
    {
        fprintf(stderr, "Using the full 9 bytes\n");
        // Need all 9 bytes
        buf[8] = (uint8_t)temp & 0xFF;
        temp >>= 8;
        for (size_t i = 7;; i--)
        {
            buf[i] = ((uint8_t)temp & 0x7F) | 0x80;
            digits -= 7;
            temp >>= 7;

            if (i == 0)
            {
                break;
            }
        }

        return 9;
    }

    size_t chunks = 1 + ((digits - 1) / 7);
    len += chunks;
    for (size_t i = chunks - 1;; i--)
    {
        if (i == chunks - 1)
        {
            // Last chunk has the high bit unset
            buf[i] = (uint8_t)temp & 0x7F;
        }
        else
        {
            // All others have the high bit set
            buf[i] = ((uint8_t)temp & 0x7F) | 0x80;
        }
        digits -= 7;
        temp >>= 7;

        if (i == 0)
        {
            break;
        }
    }

    return len;
}
