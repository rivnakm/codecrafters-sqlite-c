#include "cc-sqlite/bytes.h"
#include <stdint.h>
#include <stdio.h>

void to_be_bytes(uint8_t *dest, const uint64_t value, const size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        size_t offset = (n - 1 - i) * 8;
        dest[i] = (value >> offset) & 0xFF;
    }
}

void to_signed_be_bytes(uint8_t *dest, const int64_t value, const size_t n)
{
    if (value >= 0)
    {
        to_be_bytes(dest, (uint64_t)value, n);
        return;
    }

    uint64_t twos_comp = (uint64_t)value;
    twos_comp = ~twos_comp;
    twos_comp++;
    twos_comp |= (0x80 << (n - 1));
    to_be_bytes(dest, twos_comp, n);
}

uint64_t from_be_bytes(const uint8_t *src, const size_t n)
{
    uint64_t value = 0;
    for (size_t i = 0; i < n; i++)
    {
        value <<= 8;
        value |= src[i];
    }

    return value;
}

int64_t from_signed_be_bytes(const uint8_t *src, const size_t n)
{
    uint64_t value = src[0];
    bool positive = (value & 0x80) == 0;

    if (!positive)
    {
        uint8_t first = src[0];
        first = ~first;
        value = (uint64_t)first;
    }
    fprintf(stderr, "initial value: %04lX\n", value);

    for (size_t i = 1; i < n; i++)
    {
        value <<= 8;
        if (positive)
        {
            value |= src[i];
        }
        else
        {
            uint8_t cur = src[i];
            cur = ~cur;
            value |= (uint64_t)cur;
        }
    }

    if (positive)
    {
        return value;
    }

    value++;
    int64_t signed_val = (int64_t)value;
    signed_val = -signed_val;

    return signed_val;
}
