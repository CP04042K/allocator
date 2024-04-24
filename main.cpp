#include <MemoryPage.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>

int main() {
    MemoryPage * memory = new MemoryPage();

    memory->init();
    memory->commit();

    void * ptr1 = memory->allocate(14);
    printf("%p\n", ptr1);
    // memory->free(ptr1);

    // memory->uncommit();

    scanf("%s", (char *)ptr1);
    printf("%s", (char *)ptr1);
    memory->free(ptr1);
    memory->commit();
}