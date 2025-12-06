#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define VARINT_MAX_BYTES (9)
#define INT64_WIDTH_BYTES (8)

size_t read_varint(const uint8_t *buf, int64_t *value);
void to_be_bytes(uint8_t *dest, const uint64_t value, const size_t n);
int64_t from_signed_be_bytes(const uint8_t *src, const size_t n);

int main(void)
{
    uint8_t buffer[9] = {0xBF, 0xDB, 0xEB, 0xF4, 0xF9, 0xFC, 0xDE, 0x9F, 0x0F};
    int64_t expected = 0x7F6F5F4F3F2F1F0F;

    int64_t actual;
    size_t bytes_read = read_varint(buffer, &actual);

    printf("Expected: %ld\n", expected);
    printf("Actual: %ld\n", actual);
}

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

    uint8_t twos_comp[INT64_WIDTH_BYTES] = {0};
    to_be_bytes(twos_comp, temp, INT64_WIDTH_BYTES);
    *value = from_signed_be_bytes(twos_comp, INT64_WIDTH_BYTES);

    return len;
}

void to_be_bytes(uint8_t *dest, const uint64_t value, const size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        size_t offset = (n - 1 - i) * 8;
        dest[i] = (value >> offset) & 0xFF;
    }
}

int64_t from_signed_be_bytes(const uint8_t *src, const size_t n)
{
    if (n == 0)
    {
        return 0;
    }

    uint64_t value = src[0];
    bool positive = (value & 0x80) == 0;

    if (!positive)
    {
        uint8_t first = src[0];
        first = ~first;
        value = (uint64_t)first;
    }

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
