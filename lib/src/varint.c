#include "cc-sqlite/varint.h"

#include <stddef.h>

#include "cc-sqlite/bytes.h"

#define MAX_BE_BYTES (8)

size_t read_varint(const uint8_t *buf, uint64_t *value)
{
    uint8_t a0 = buf[0];

    if (a0 <= 240)
    {
        *value = a0;
        return 1;
    }
    else if (a0 <= 248)
    {
        *value = 240 + (256 * (buf[0] - 241)) + buf[1];
        return 2;
    }
    else if (a0 == 249)
    {
        *value = 2288 + (256 * buf[1]) + buf[2];
        return 3;
    }
    else
    {
        size_t val_bytes = (a0 - 250) + 3;
        *value = from_be_bytes(&(buf[1]), val_bytes);
        return val_bytes + 1;
    }
}

size_t write_varint(uint8_t *buf, const uint64_t value)
{
    if (value <= 240)
    {
        buf[0] = value;
        return 1;
    }
    else if (value <= 2287)
    {
        buf[0] = ((value - 240) / 256) + 241;
        buf[1] = (value - 240) % 256;
        return 2;
    }
    else if (value <= 67823)
    {
        buf[0] = 249;
        buf[1] = (value - 2288) / 256;
        buf[2] = (value - 2288) % 256;
        return 3;
    }
    else if (value <= 16777215)
    {
        buf[0] = 250;
        to_be_bytes(&(buf[1]), value, 3);
        return 4;
    }
    else if (value <= 4294967295)
    {
        buf[0] = 251;
        to_be_bytes(&(buf[1]), value, 4);
        return 5;
    }
    else if (value <= 1099511627775)
    {
        buf[0] = 252;
        to_be_bytes(&(buf[1]), value, 5);
        return 6;
    }
    else if (value <= 281474976710655)
    {
        buf[0] = 253;
        to_be_bytes(&(buf[1]), value, 6);
        return 7;
    }
    else if (value <= 72057594037927935)
    {
        buf[0] = 254;
        to_be_bytes(&(buf[1]), value, 7);
        return 8;
    }
    else
    {
        buf[0] = 255;
        to_be_bytes(&(buf[1]), value, 8);
        return 9;
    }
}
