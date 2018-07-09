internal Buffer
read_entire_file(char *filename)
{
    Buffer result = {0};

    FILE *file = fopen(filename, "rb");
    // NOTE(michiel): Get the file size
    fseek(file, 0, SEEK_END);
    result.size = safe_truncate_to_u32(ftell(file));
    // NOTE(michiel): Reset the current file pointer to the beginning
    fseek(file, 0, SEEK_SET);
    // NOTE(michiel): Allocate memory for the file data
    result.data = allocate_array(result.size, u8, 0);
    // NOTE(michiel): Read the actual data from the file
    s64 bytesRead = fread(result.data, 1, result.size, file);
    i_expect(bytesRead == (s64)result.size);
    fclose(file);

    return result;
}

internal u32
string_length(char *cString)
{
    u32 length = 0;
    while (*cString++)
    {
        ++length;
    }
    return length;
}

internal inline String
create_string(char *cString)
{
    String result = {0};
    result.size = string_length(cString);
    result.data = (u8 *)cString;
    return result;
}

internal b32
strings_are_equal(String a, String b)
{
    b32 result = false;

    if (a.size == b.size)
    {
        result = true;
        for (u32 nameIndex = 0; nameIndex < a.size; ++nameIndex)
        {
            if (a.data[nameIndex] != b.data[nameIndex])
            {
                result = false;
                break;
            }
        }
    }

    return result;
}

