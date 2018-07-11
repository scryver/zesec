#include "../src/common.h"
#include "../src/common.c"

typedef struct Bammy
{
    u32 x;
    f32 y;
    s64 z;
} Bammy;

int test_auto_buffer(void)
{
    Bammy *myBuf = NULL;
    i_expect(buf_len(myBuf) == 0);
    i_expect(buf_cap(myBuf) == 0);

    Bammy henk = {12, -4.5f, -5943};
    buf_push(myBuf, henk);
    i_expect(buf_len(myBuf) == 1);
    i_expect(buf_cap(myBuf) >= 1);
    i_expect(myBuf[0].x == 12);
    i_expect(myBuf[0].y == -4.5f);
    i_expect(myBuf[0].z == -5943);

    // NOTE(michiel): Add to end of buffer and return the start of the new block
    Bammy *allthenew = buf_add(myBuf, 110);
    i_expect(buf_len(myBuf) == 111);
    i_expect(buf_cap(myBuf) >= 111);
    i_expect(allthenew == (myBuf + 1));

    buf_last(myBuf) = (Bammy){1, -2.0f, 3};
    i_expect(myBuf[110].x == 1);
    i_expect(myBuf[110].y == -2.0f);
    i_expect(myBuf[110].z == 3);

    buf_clear(myBuf);
    i_expect(buf_len(myBuf) == 0);

    myBuf = buf_free(myBuf);
    i_expect(myBuf == 0);
    return 0;
}

int test_auto_map(void)
{
    Map *bammyMap = &(Map){0};
    Bammy harry = {1, -3.0f, 2};

    i_expect(map_get(bammyMap, &harry) == 0);
    map_put(bammyMap, &harry, &harry);
    Bammy *gotHim = map_get(bammyMap, &harry);
    i_expect(gotHim);
    i_expect(gotHim->x == harry.x);
    i_expect(gotHim->y == harry.y);
    i_expect(gotHim->z == harry.z);

    return 0;
}

int test_common(void)
{
    int errors = 0;
    errors |= test_auto_buffer();
    errors |= test_auto_map();
    return errors;
}

int main(int argc, char **argv)
{
    int errors = test_common();
    return errors;
}
