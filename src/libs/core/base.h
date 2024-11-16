#include <stdbool.h>
#include <stdint.h>

typedef int8_t          i8;
typedef int16_t         i16;
typedef int32_t         i32;
typedef int64_t         i64;
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
typedef i64             isize;
typedef usize           size_t;
typedef isize           ssize_t;

#define len(x)      sizeof(x) / sizeof((x)[0])
// FIXME: Different definition depending on compiler?
#define noreturn    __attribute__((noreturn))
#define null        (void *)0
// FIXME: Different definition depending on compiler?
#define packed      __attribute__((packed))

#define KiB         * 1024
#define MiB         * 1024 * 1024
#define GiB         * 1024 * 1024 * 1024
