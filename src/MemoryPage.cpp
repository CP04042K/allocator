#include <sys/mman.h>
#include <MemoryPage.h>
#include <Allocator.h>
#include <cstdint>

#define kMinorGcYoungSpaceSize 0x2000
#define kMajorGcOldSpaceSize 0x20000
#define kPageAllocationFailure (void *)0xb
#define kMemoryMmapFaiure 0xa

MemoryPage::MemoryPage() {
    this->base_mem_ptr = nullptr;
    this->mem_ptr = nullptr;
    this->in_use = true;
    this->mem_size = 0;
    this->allocatedBytes = 0;
    this->allocator = new Allocator(this);
    this->bin = nullptr;
}

int MemoryPage::init(bool is_major_gc) {
    void * ptr = nullptr;
    if (is_major_gc) {
        ptr = mmap(nullptr, kMajorGcOldSpaceSize, PROT_NONE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
        this->mem_size = kMajorGcOldSpaceSize;
    } else {
        ptr = mmap(nullptr, kMinorGcYoungSpaceSize, PROT_NONE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
        this->mem_size = kMinorGcYoungSpaceSize;
    }

    if (ptr == MAP_FAILED) return kMemoryMmapFaiure; 

    this->base_mem_ptr = ptr;
    this->mem_ptr = this->base_mem_ptr;

    return kMemoryInitSuccess;
}

void * MemoryPage::allocate(uint64_t size) {
    if (size >= this->mem_size) return kPageAllocationFailure;

    return this->allocator->allocate(size);
}

int MemoryPage::free(void * ptr) {
    return this->allocator->free(ptr);
}

bool MemoryPage::isSpaceAvailable(uint64_t size) {
    if ((void *)((uint64_t)this->mem_ptr + size) > (void *)((uint64_t)this->base_mem_ptr+this->mem_size)) {
        // TODO: GC and check again, if still not enough space then return false
        return false;
    }

    return true;
}

void * MemoryPage::giveSpace(uint64_t size) {
    if ((void *)((uint64_t)this->mem_ptr + size) > (void *)((uint64_t)this->base_mem_ptr+this->mem_size)) return kNotEnoughSpace;

    void * ptr = this->mem_ptr;
    this->mem_ptr = (void *)((uint64_t)(this->mem_ptr) + size + kMemoryAddressSize); // ensure mem_ptr always point to next free memory
    return ptr;
}

void MemoryPage::uncommit() {
    mprotect(this->base_mem_ptr, this->mem_size, PROT_NONE);
}

void MemoryPage::commit() {
    mprotect(this->base_mem_ptr, this->mem_size, PROT_READ | PROT_WRITE);
}