#include "memory.h"

void *memcpy(void *dest, const void *src, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        ((byte *)dest)[i] = ((const byte *)src)[i];
    }

    return dest;
}

void *memset(void *p, int value, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        ((byte *)p)[i] = value;
    }

    return p;
}

void *memmove(void *dest, const void *src, size_t n) {
    if (src > dest) {
        for (size_t i = 0; i < n; ++i) {
            ((byte *)dest)[i] = ((const byte *)src)[i];
        }
    } else if (src < dest) {
        for (size_t i = n; i > 0; --i) {
            ((byte *)dest)[i-1] = ((const byte *)src)[i-1];
        }
    }

    return dest;
}

int memcmp(const void *p1, const void *p2, size_t n) {
    const byte *bp1 = p1;
    const byte *bp2 = p2;
    for (size_t i = 0; i < n; ++i) {
        if (bp1[i] != bp2[i]) {
            return bp1[i] < bp2[i] ? -1 : 1;
        }
    }

    return 0;
}

void memory_copy(void *dest, const void *src, usize size) {
    memcpy(dest, src, size);
}

void memory_move(void *dest, const void *src, usize size) {
    memmove(dest, src, size);
}

void memory_set(void *p, byte value, usize size) {
    memset(p, value, size);
}
