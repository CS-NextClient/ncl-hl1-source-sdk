#include <cstring>

void* Mem_ZeroMalloc(size_t bytes)
{
    char* buffer = new char[bytes];
    std::memset(buffer, 0, bytes);

    return buffer;
}

void Mem_Free(void* buffer)
{
    delete[] (char*)buffer;
}