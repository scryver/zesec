#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define internal  static
#define global    static
#define persist   static
#define unused(x) (void)x

#define i_expect  assert
#define INVALID_DEFAULT_CASE default: { i_expect(0 && "Invalid default case"); } break

#define U8_MAX    0xFF
#define U16_MAX   0xFFFF
#define U32_MAX   0xFFFFFFFF
#define U64_MAX   0xFFFFFFFFFFFFFFFF

typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;

typedef int8_t    s8;
typedef int16_t   s16;
typedef int32_t   s32;
typedef int64_t   s64;

typedef int8_t    b8;
typedef int16_t   b16;
typedef int32_t   b32;
typedef int64_t   b64;

typedef float     f32;
typedef double    f64;

typedef size_t    uptr;

typedef struct Buffer
{
    u32 size;
    u8 *data;
} Buffer;
typedef Buffer String;

typedef enum AllocateFlags
{
    ALLOC_NOCLEAR = 0x01,  // NOTE(michiel): This is a NO so a flag value of 0 clears the memory by default
} AllocateFlags;

internal inline u32 safe_truncate_to_u32(u64 value)
{
    i_expect(value <= U32_MAX);
    return (u32)(value & U32_MAX);
}

#define allocate_array(count, type, flags) (type *)allocate_size(sizeof(type) * count, flags)
#define allocate_struct(type, flags) (type *)allocate_size(sizeof(type), flags)
internal inline void *allocate_size(u32 size, u32 flags)
{
    void *result = NULL;

    b32 clear = !(flags & ALLOC_NOCLEAR);
    if (clear)
    {
        result = calloc(size, 1);
    }
    else
    {
        result = malloc(size);
    }

    return result;
}

internal inline Buffer allocate_buffer(u32 size, u32 flags)
{
    Buffer result = {.size = size};
    result.data = allocate_array(size, u8, flags);
    if (result.data == NULL)
    {
        result.size = 0;
    }
    return result;
}

internal inline void deallocate(void *mem)
{
    i_expect(mem);
    free(mem);
}
