#include <cstdint>

#define IN_USE_MARKBIT 0x0100000000000000

class MemoryBlock
{
public:
    uint64_t block_size;

};

class FreedBlock : public MemoryBlock
{
public:
    FreedBlock * next;
    FreedBlock * prev;
    FreedBlock(uint64_t size);
    ~FreedBlock();
};

class AllocatedBlock : public MemoryBlock
{
public:
    long unused[2]; // padding 
    AllocatedBlock(uint64_t size);
    ~AllocatedBlock();
};
