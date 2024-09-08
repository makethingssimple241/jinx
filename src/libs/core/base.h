#include <stdbool.h>
#include <stdint.h>

typedef uint8_t         u8;
typedef uint8_t         byte;
typedef uint16_t        u16;
typedef uint16_t        word;
typedef uint32_t        u32;
typedef uint32_t        dword;
typedef uint64_t        u64;
typedef uint64_t        qword;

typedef unsigned int    uint;

// FIXME: Different size depending on architecture?
typedef u64             usize;
typedef usize           size_t;

#define noreturn    __attribute__((noreturn))
#define packed      __attribute__((packed))
#define null        (void *)0
