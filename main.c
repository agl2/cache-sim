#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cache.h"
#include "memory.h"
#define CACHE_SIZE 32
#define BLOCK_SIZE 4
#define NUMBER_OF_SETS 1
#define MEMORY_INPUT_FN "mem_gen/mem.dmp"
#define MEMORY_OUTPUT_FN "mem_output.dmp"
#define CACHE_OUTPUT_FN "cache.dmp"
#define CACHE_DUMP_FILE
#define MAIN_MEM_SIZE 128

int main()
{
    cache_t cache;
    byte_t* main_mem;

    set_cache_params(&cache, CACHE_SIZE, BLOCK_SIZE, NUMBER_OF_SETS);
    init_cache(&cache);

    printf("word size: %d bits\n", sizeof(word_t)*8);
    printf("byte size: %d bits\n", sizeof(byte_t)*8);
    printf("mem_addr_t size: %d bits\n", sizeof(mem_addr_t)*8);
    printf("boolean size: %d bits\n", sizeof(boolean)*8);

    main_mem = mem_load(MEMORY_INPUT_FN, MAIN_MEM_SIZE);

    cache_dump_file(&cache);

    mem_dump_file(main_mem, MEMORY_OUTPUT_FN, MAIN_MEM_SIZE);
    return 0;
}
