#include <cstdint>
#define kMemoryInitFailure ~1
#define kMemoryInitSuccess 1
#define kInitialGlobalObjectListLength 10

#define kMemoryAddressSize 8

class Allocator;
class FreedBlock;

class MemoryPage {
    private:
        Allocator * allocator;
        // TODO: when limit reached, allocate a new chunk to save global objects
        // MemoryBlock node;
    public:
        uint64_t allocatedBytes;
        uint64_t mem_size;
        short bin_size;
        void * base_mem_ptr;
        void * mem_ptr;
        bool in_use;
        FreedBlock * bin;

        int init(bool is_major_gc = false);
        void * allocate(uint64_t size);
        int free(void * ptr);
        // void * copyObjectToAllocated(void * ptr, HeapObject obj);
        bool isSpaceAvailable(uint64_t size);
        void * giveSpace(uint64_t size);
        void uncommit();
        void commit();
        MemoryPage();
};