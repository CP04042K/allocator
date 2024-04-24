// #include <MemoryPage.h>
#include <cstdint>

class FreedBlock;

#define kNotEnoughSpace (void *)-0xc

class MemoryPage;

class Allocator
{
public:
    MemoryPage * memory;
    void * requestMemory(uint64_t size);
    void * allocate(uint64_t size);
    void markInUse(uint64_t &size);
    int free(void * ptr);
    bool isFreedBlock(void * ptr);
    uint64_t getUnmaskedSize(uint64_t size);
    uint64_t align(uint64_t size);
    FreedBlock * checkBin(uint64_t size);
    Allocator(MemoryPage *memory);
    Allocator();
    ~Allocator();
};