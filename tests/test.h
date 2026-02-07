#include "../src/cpu.c"

#include <stdlib.h>
#include <assert.h>
#include <time.h>

#define KB 1024
#define MB (1024*KB)
#define GB (1024*MB)

#define MEMORY_CAP (1*GB)

void* xmalloc(size_t size) {
    void* result = malloc(size);
    assert(result && "https://www.amazon.com/CORSAIR-VENGEANCE-6400MHz-Compatible-Computer/dp/B0BXHC74WD");
    return result;
}

struct CpuContext create_ctx(void) {
    void* memory = xmalloc(MEMORY_CAP);
    return (struct CpuContext) {
        .memory = memory,
        .memory_len = MEMORY_CAP,
    };
}

void random_init_buffer(word_t* buffer, size_t len) {
    for (size_t i = 0; i < len; i++) {
        buffer[i] = rand() % 1000;
    }
}

void swap_words(word_t* a, word_t* b) {
    word_t const temp = *a;
    *a = *b;
    *b = temp;
}

void shuffle_buffer(word_t* buffer, size_t len) {
    for (size_t i = 0; i < len; i++) {
        size_t const j = rand() % len;
        swap_words(&buffer[i], &buffer[j]);
    }
}
