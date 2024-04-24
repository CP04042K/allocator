#include <Allocator.h>
#include <MemoryPage.h>
#include <MemoryBlock.h>
#include <cstring>

#define kFreeingInvalidMemoryChunk -0xd
#define kMaxBinSize 64

uint64_t Allocator::align(uint64_t size) {
  return (size + sizeof(__WORDSIZE) - 1) & ~(sizeof(__WORDSIZE) - 1);
}

void * Allocator::requestMemory(uint64_t size) {
    size = this->align(size);
    if (!this->memory->isSpaceAvailable(size)) return kNotEnoughSpace;

    void * ptr = this->memory->giveSpace(size);

    if (ptr == kNotEnoughSpace) return kNotEnoughSpace;

    return ptr;

}

void * Allocator::allocate(uint64_t size) {
    size = this->align(size);
    uint64_t blockHeaderSize = sizeof(AllocatedBlock);
    void * ptr;
    AllocatedBlock * block;

    if ((ptr = this->checkBin(size))) {
        block = new AllocatedBlock(((FreedBlock *)ptr)->block_size);
        this->markInUse(block->block_size);
        
        memcpy(ptr, block, ((FreedBlock *)ptr)->block_size+blockHeaderSize);
        return (void *)((uint64_t)ptr + blockHeaderSize);
    }

    block = new AllocatedBlock(size);
    this->markInUse(block->block_size);

    ptr = this->requestMemory(size+blockHeaderSize);
    if (ptr == kNotEnoughSpace) return kNotEnoughSpace;

    memcpy(ptr, block, size+blockHeaderSize);

    ptr = (void*)((uint64_t)ptr+blockHeaderSize);
    memset(ptr, '\0', size);

    this->memory->allocatedBytes += size+blockHeaderSize;

    return ptr;

}

int Allocator::free(void * ptr) {
    uint64_t block_size = *((uint64_t *)((char *)ptr - 24));
    FreedBlock * block = new FreedBlock(this->getUnmaskedSize(block_size));

    if (this->isFreedBlock(ptr)) return kFreeingInvalidMemoryChunk;
    memcpy((char *)ptr-24, block, sizeof(FreedBlock));

    if (this->memory->bin_size < kMaxBinSize) {
        if (this->memory->bin == nullptr) {
            this->memory->bin = (FreedBlock *)((char *)ptr-24);

        } else {
            FreedBlock * tmp_bin = this->memory->bin;
            while (true) {
                if (!tmp_bin) break;
                if (tmp_bin->next) {
                    tmp_bin = tmp_bin->next;
                } else break;
            }
            tmp_bin->next = (FreedBlock *)((char *)ptr-24);
            tmp_bin->next->prev = tmp_bin;
        }
        ++this->memory->bin_size;
    }

    memset(ptr, '\0', this->getUnmaskedSize(block_size));
    return 1;
}

FreedBlock * Allocator::checkBin(uint64_t size) {
    if (this->memory->bin == NULL) return NULL;

    FreedBlock * block = this->memory->bin;
    FreedBlock * ret_block = NULL;
    long cacheBlock = 0; 

    size_t blockCount = 0;
    size_t _blockSize = 0;
    int _blockCount = 0;
    while (true) {
        if (block->block_size >= size) {
            if (block->block_size == size) {
                ret_block = block;

                if (this->memory->bin == ret_block) {
                    this->memory->bin = ret_block->next;
                }

                if (ret_block->prev) {
                    ret_block->prev->next = ret_block->next;
                } 

                if (ret_block->next) {
                    ret_block->next->prev = ret_block->prev;
                }
                break;
            } else {
                if (cacheBlock) {
                    _blockCount = cacheBlock >> 32;
                    _blockSize = cacheBlock & 0xffffffff;
                    if (_blockSize > block->block_size) {
                        cacheBlock = 0;
                        cacheBlock = (cacheBlock | (blockCount << 32)) | block->block_size; // <----32bit----><----32bit---->
                                                                                            //    blockCount     blockSize
                    }
                } else {
                    cacheBlock = 0;                                            
                    cacheBlock = (cacheBlock | (blockCount << 32)) | block->block_size;
                }
            }
        }

        if (!block->next) break;

        block = block->next;
        ++blockCount;
    }

    blockCount = 0;

    if (ret_block != NULL) {
        return ret_block;
    } 

    if (ret_block == NULL) {
        if (cacheBlock == 0L) return NULL;
        ret_block = this->memory->bin;

        if (cacheBlock) {
            _blockCount = cacheBlock >> 32;

            for (int i = 0; i < _blockCount; ++i) {
                ret_block = block->next;
            }

            if (this->memory->bin == ret_block) {
                this->memory->bin = ret_block->next;
            }

            if (ret_block->prev) {
                ret_block->prev->next = ret_block->next;
            } 

            if (ret_block->next) {
                ret_block->next->prev = ret_block->prev;
            }

            return ret_block;
        }
    } 

    return NULL;
}

void Allocator::markInUse(uint64_t &size) {
    size |= IN_USE_MARKBIT;
}

uint64_t Allocator::getUnmaskedSize(uint64_t size) {
    return size ^ IN_USE_MARKBIT;
}


bool Allocator::isFreedBlock(void * ptr) {
    uint64_t block_size = *((uint64_t *)((char *)ptr - 24));
    bool isFreed = !(block_size >> 56);

    return isFreed;
}

Allocator::Allocator(MemoryPage * memory)
{
    this->memory = memory;
}

Allocator::~Allocator()
{
}
