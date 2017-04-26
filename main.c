#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cache.h"
#include "memory.h"
#define CACHE_SIZE 32
#define BLOCK_SIZE 4
#define NUMBER_OF_SETS 2
#define MEMORY_INPUT_FN "mem_gen/mem.dmp"
#define MEMORY_OUTPUT_FN "mem_output.dmp"
#define CACHE_OUTPUT_FN "cache.dmp"
#define MAIN_MEM_SIZE 1024

void address_split_show(mem_addr_t addr) {
    unsigned tag = addr >> (cache->n_line_bits + cache->n_offset_bits);
    unsigned line = (addr << cache->n_tag_bits) >> (cache->n_tag_bits + cache->n_offset_bits);
    unsigned offset = (addr << (cache->n_tag_bits + cache->n_line_bits)) >> (cache->n_tag_bits + cache->n_line_bits);

    printf("tag: %x\n", tag);
    printf("line: %x\n", line);
    printf("offset: %x\n", offset);
}

int main()
{
    cache_t* cache = (cache_t*) malloc(sizeof(cache_t));
    byte_t* main_mem;

    set_cache_params(cache, CACHE_SIZE, BLOCK_SIZE, NUMBER_OF_SETS);
    init_cache(cache);

    printf("word size: %d bits\n", sizeof(word_t)*8);
    printf("byte size: %d bits\n", sizeof(byte_t)*8);
    printf("mem_addr_t size: %d bits\n", sizeof(mem_addr_t)*8);
    printf("boolean size: %d bits\n", sizeof(boolean)*8);
    printf("tag size: %d bits\n", cache->n_tag_bits);
    printf("block size: %d bits\n", cache->n_line_bits);
    printf("offset size: %d bits\n", cache->n_offset_bits);


    main_mem = mem_load(MEMORY_INPUT_FN, MAIN_MEM_SIZE);



    /*for(int i = 0; i < 64; i++) {
        printf("Random Replace: %d\n", i);
        mem_addr_t addr = (mem_addr_t) rand()%MAIN_MEM_SIZE;
        if(!find_block(cache, &addr, MAIN_MEM_SIZE)) {
            printf("Address: %.8x not in memory range", addr);
        }
        random_replace(main_mem, cache, addr);
    }*/

    /*for(int i = 0; i < 64; i++) {
        printf("Fifo Replace: %d\n", i);
        mem_addr_t addr = (mem_addr_t) rand()%MAIN_MEM_SIZE;
        if(!find_block(cache, &addr, MAIN_MEM_SIZE)) {
            printf("Address: %.8x not in memory range", addr);
            exit(3);
        }
        fifo_replace(main_mem, cache, addr);
    }*/

    cache_dump_file(cache);
    mem_dump_file(main_mem, MEMORY_OUTPUT_FN, MAIN_MEM_SIZE);
    return 0;
}
