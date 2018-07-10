#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <memory.h>
#include <assert.h>

#define internal  static
#define global    static
#define persist   static
#define unused(x) (void)x

#define false     0
#define true      1

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

// TODO(michiel): Own struct for this
typedef struct FileStream
{
    FILE *file;
} FileStream;

internal inline u32 safe_truncate_to_u32(u64 value)
{
    i_expect(value <= U32_MAX);
    return (u32)(value & U32_MAX);
}

typedef enum AllocateFlags
{
    ALLOC_NOCLEAR = 0x01,  // NOTE(michiel): This is a NO so a flag value of 0 clears the memory by default
} AllocateFlags;

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

internal inline void *
reallocate_size(void *data, u32 size)
{
    void *result = NULL;
    result = realloc(data, size);
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

typedef struct BufferHeader
{
    u64 magic;
    u32 len;
    u32 cap;
    u8 data[];
} BufferHeader;

#define BUF_MAGIC 0x457ABF36A4ED8654

#define buf_free(buf)           ((buf) ? deallocate(buf_hdr(buf)), NULL : NULL)
#define buf_clear(buf)          ((buf) ? (buf_len_(buf) = 0, (buf)) : 0)
#define buf_push(buf, val)      (buf_maybegrow(buf, 1), (buf)[buf_len_(buf)++] = (val))
#define buf_add(buf, n)         (buf_maybegrow(buf, n), buf_len_(buf) += (n), &(buf)[buf_len_(buf) - (n)])
#define buf_last(buf)           ((buf)[buf_len(buf) - 1])
#define buf_len(buf)            ((buf) ? buf_len_(buf) : 0)
#define buf_cap(buf)            ((buf) ? buf_cap_(buf) : 0)

#define buf_hdr(buf)            ((BufferHeader *)((u8 *)(buf) - offsetof(BufferHeader, data)))
#define buf_len_(buf)           (buf_hdr(buf)->len)
#define buf_cap_(buf)           (buf_hdr(buf)->cap)

#define buf_needgrow(buf, len)  (((buf) == 0) || ((buf_len_(buf) + (len)) >= buf_cap_(buf)))
#define buf_maybegrow(buf, len) (buf_needgrow(buf, (len)) ? buf_grow(buf, len), 0 : 0)

#define BUFFER_ADDRESS_MOD 1

#if BUFFER_ADDRESS_MOD
#define buf_grow(buf, len)      (buf_grow_((void **)&(buf), (len), sizeof(*(buf))))
#else
#define buf_grow(buf, len)      (*((void **)&(buf)) = buf_grow_((buf), (len), sizeof(*(buf))))
#endif // BUFFER_ADDRESS_MOD
//#define buf_put(buf, val)  (buf_put_(&buf, val, 1, sizeof(*buf)))

#if BUFFER_ADDRESS_MOD
internal inline void
buf_grow_(void **bufPtr, u32 increment, u32 elemSize)
#else
internal inline void *
buf_grow_(void *buf, u32 increment, u32 elemSize)
#endif
{
#if BUFFER_ADDRESS_MOD
    void *buf = *bufPtr;
#endif
    i_expect(buf_cap(buf) <= ((U32_MAX - 1) / 2));
    u32 newCap = 2 * buf_cap(buf);
    u32 newLength = buf_len(buf) + increment;
    if (newCap < 16)
    {
        newCap = 16;
    }
    if (newCap < newLength)
    {
        newCap = newLength;
    }
    i_expect(newLength <= newCap);
    i_expect(newCap <= ((U32_MAX - offsetof(BufferHeader, data)) / elemSize));
    u32 newSize = offsetof(BufferHeader, data) + newCap * elemSize;
    BufferHeader *newHeader = reallocate_size(buf ? buf_hdr(buf) : 0, newSize);
    i_expect(newHeader);
    if (!buf)
    {
        newHeader->len = 0;
    }
    newHeader->cap = newCap;
#if BUFFER_ADDRESS_MOD
    *bufPtr = newHeader->data;
#else
    return newHeader->data;
#endif
}

#if 0
internal inline void
buf_put_(void **bufPtr, void *data, u32 newLength, u32 elemSize)
{
    void *buf = *bufPtr;
    if (buf_len(buf) == buf_cap(buf))
    {
        buf_grow_(buf, newLength, elemSize);
    }
    memcpy((u8 *)buf + buf_hdr(buf)->len * elemSize, data, newLength * elemSize);
    buf_hdr(buf)->len += newLength;
}
#endif

typedef struct Map
{
    u32 size;
    void **keys;
    void **values;
} Map;

internal inline u32
get_hash(uptr key, u32 maxSize)
{
    i_expect(maxSize);
    u32 hash = (key >> 32) ^ (key & U32_MAX);
    hash = (hash << 17) + hash;
    hash = hash % maxSize;
    return hash;
}

internal inline void map_grow(Map **mapPtr);

internal inline void *
map_get(Map *map, void *key)
{
    i_expect(key);
    if (map->size == 0)
    {
        return 0;
    }
    u32 hash = get_hash((uptr)key, map->size);

    u32 index = hash;
    while (map->keys[index] != key)
    {
        ++index;
        if (index >= map->size)
        {
            index = 0;
        }

        if (index == hash)
        {
            // NOTE(michiel): Complete loop, so not found
            return 0;
        }
    }

    return map->values + index;
}

#define map_put(map, key, value) map_put_(&(map), key, &value, sizeof(value))
internal inline b32
map_put_(Map **mapPtr, void *key, void *value, u64 itemSize)
{
    Map *map = *mapPtr;
    i_expect(key);
    if (map->size == 0)
    {
        map_grow(&map);
    }
    u32 hash = get_hash((uptr)key, map->size);

    u32 index = hash;
    while (map->keys[index] != 0)
    {
        ++index;
        if (index >= map->size)
        {
            index = 0;
        }

        if (index == hash)
        {
            // NOTE(michiel): Complete loop, we're full
            map_grow(&map);
            return map_put_(&map, key, value, itemSize);
        }
    }

    map->keys[index] = key;
    memcpy(map->values + index, value, itemSize);

    *mapPtr = map;

    return 0;
}

internal inline void
map_grow(Map **mapPtr)
{
    Map *map = *mapPtr;
    Map *newMap = &(Map){0};
    newMap->size = map->size ? map->size * 2 : 1024;
    newMap->keys = (void **)allocate_array(newMap->size, uptr, 0);
    newMap->values = (void **)allocate_array(newMap->size, uptr, 0);
    for (u32 mapIndex = 0; mapIndex < map->size; ++mapIndex)
    {
        if (map->keys[mapIndex])
        {
            map_put(newMap, map->keys[mapIndex], map->values[mapIndex]);
        }
    }
    if (map->keys)
    {
        deallocate(map->keys);
    }
    if (map->values)
    {
        deallocate(map->values);
    }
    *mapPtr = newMap;
}
