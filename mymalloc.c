/*lawend tash 21120205702
   Mehmet Dağlı 22120205382
   Mert Akmahmut 23120205082*/

#include "mymalloc.h"
#include <stdio.h>
#include <unistd.h>

#define BLOCK_SIZE 16

/** istenilen stratejiye göre bir tane free block bulunup,
 * bu alan iki bloğa split edilerek
 * allocates memory,
 * başlangıç adresi (data[]) return edilir.
 */
void *mymalloc(size_t size) { 
    if (size <= 0) return NULL;

    // çoklu BLOCK_SIZE'ını en yakın değere yuvarlanır
    size_t size_b= (size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    // uygun bir yer bulmak için free list'i dolaşmak
    Block *current = free_list;
    Block *prev = NULL;
    while (current != NULL) {
        if (current->info.size >= size_b) {
            // Uygun blok bulunursa
            if (current->info.size > size_b) {
                // blok gereğinden fazla büyükse split yapılır
                Block *split = split_block(current, size_b);
                return (void *)(split->data);
            } else {
                // bloğu free list'ten silmek
                if (prev != NULL) {
                    prev->next = current->next;
                } else {
                    free_list = current->next;
                }
                current->info.isfree = 0;
                return (void *)(current->data);
            }
        }
        prev = current;
        current = current->next;
    }

    // eğer boş blok bulunmazsa memory allocate yapıldı
    size_t expand_size = size_b * BLOCK_SIZE;
    if (heap_end == NULL) {
        // mymalloc'a ilk call etmek veya heap space allocate edilmemişse call etmek
        free_list = sbrk(expand_size);
        if (free_list == (void *)-1) return NULL;
        free_list->info.size = size_b;
        free_list->info.isfree = 0;
        free_list->next = NULL;
        heap_start = free_list;
        heap_end = free_list + size_b;
        return (void *)(free_list->data);
    } else {
        // heap space genişletmek
        current = sbrk(expand_size);
        if (current == (void *)-1) return NULL;
        current->info.size = size_b;
        current->info.isfree = 0;
        current->next = NULL;
        heap_end->next = current;
        heap_end = current;
        return (void *)(current->data);
    }
}

/** block free edildi,
 * Eğer coalescing(birleştirme) yapılabiliyorsa (bir önceki ve bir sonraki blokla) yapıldı.
 * free list ayarlandı
 */
void myfree(void *p) {
    if (p == NULL) return;

    // Get the block header
    Block *block = (Block *)((char *)p - sizeof(Tag));

    // block free olarak işaretlendi
    block->info.isfree = 1;

    // blok free liste ekle
    if (listtype == ADDR_ORDERED_LIST) {
        // blok insert etmek için doğru pozisyon bulunur
        Block *current = free_list;
        Block *prev = NULL;
        while (current != NULL && current < block) {
            prev = current;
            current = current->next;
        }
        // insert block
        if (prev == NULL) {
            block->next = free_list;
            free_list = block;
        } else {
            prev->next = block;
            block->next = current;
        }
    } else {
        // LIFO insertion
        block->next = free_list;
        free_list = block;
    }

    // bir önceki ve bir sonraki blokla birleştirme
    Block *coalesced_block = left_coalesce(block);
    right_coalesce(coalesced_block);
}
/** size kullanarak(16 byte blok), blok bölünür
 * returns the left block,
 * free list'e gerekli ayarlanmalar yapılır
 */
Block *split_block(Block *b, size_t size) {
    if (b->info.size <= size) return b;

    Block *split = (Block *)((char *)b + size * BLOCK_SIZE);
    split->info.size = b->info.size - size;
    split->info.isfree = 1;
    split->next = b->next;
    b->info.size = size;
    b->info.isfree = 0;
    b->next = split;
    return b;
}

/** b bloğunu bir öncekiyle birleştirme,
 * returns the final block
 */
Block *left_coalesce(Block *b) {
    if (b == NULL) return NULL;
    if (b->next == NULL) return b;

    Block *next = b->next;
    if (b->info.isfree && next->info.isfree) {
        b->info.size += next->info.size;
        b->next = next->next;
    }
    return b;
}

/** b bloğun bir sonrasıyla birleştirme,
 * returns the final block
 */
Block *right_coalesce(Block *b) {
    if (b == NULL) return NULL;

    Block *current = free_list;
    Block *prev = NULL;
    while (current != NULL && current < b) {
        prev = current;
        current = current->next;
    }
    if (prev != NULL && (char *)prev + prev->info.size * BLOCK_SIZE == (char *)b) {
        prev->info.size += b->info.size;
        prev->next = b->next;
        return prev;
    }
    return b;
}
/** prints data of the blocks
 * --------
 * size:
 * free:
 * --------
 */
void printheap() {
    Block *current = heap_start;
    while (current != NULL) {
        printf("---------------\n");
        printf("Free: %d\n", current->info.isfree);
        printf("Size: %llu\n", current->info.size * BLOCK_SIZE);
        printf("---------------\n");
        current = current->next;
    }
}

void print_external_fragmentation() {
    Block *current = free_list;
    uint64_t total_free_blocks = 0;
    uint64_t total_free_size = 0;
    while (current != NULL) {
        total_free_blocks++;
        total_free_size += current->info.size;
        current = current->next;
    }
    printf("External Fragmentation: %llu blocks, %llu bytes\n", total_free_blocks, total_free_size * BLOCK_SIZE);
}

int main() {
    // Test 1: Allocate and free a small block
    printf("**FARKLI STRATEJİLERDE ALLOCATE AND FREE SENARYOLARI**");
    printf("\nTest 1: Allocate and free a small block\n");
    printf("Before allocation:\n");
    printheap();
    printf("External Fragmentation:\n");
    print_external_fragmentation();

    int *ptr1 = (int *)mymalloc(sizeof(int));
    if (ptr1 != NULL) {
        *ptr1 = 10;
        printf("Value: %d\n", *ptr1);
        myfree(ptr1);
    }

    printf("After allocation and free:\n");
    printheap();
    printf("External Fragmentation:\n");
    print_external_fragmentation();

    // Test 2: Allocate a large block
    printf("\nTest 2: Allocate a large block\n");
    printf("Before allocation:\n");
    printheap();
    printf("External Fragmentation:\n");
    print_external_fragmentation();

    int *ptr2 = (int *)mymalloc(1024 * sizeof(int));
    if (ptr2 != NULL) {
        *ptr2 = 20;
        printf("Value: %d\n", *ptr2);
    }

    printf("After allocation:\n");
    printheap();
    printf("External Fragmentation:\n");
    print_external_fragmentation();

    // Test 3: Free the large block
    printf("\nTest 3: Free the large block\n");
    printf("Before free:\n");
    printheap();
    printf("External Fragmentation:\n");
    print_external_fragmentation();

    myfree(ptr2);

    printf("After free:\n");
    printheap();
    printf("External Fragmentation:\n");
    print_external_fragmentation();

    return 0;
}
