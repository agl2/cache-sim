#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cache.h"
#include "memory.h"

/**============ CACHE PARAMETERS =============*/
#define NUMBER_OF_SETS 2
#define CACHE_SIZE 32
#define BLOCK_SIZE 4
#define NUMBER_OF_SETS 2
#define FIFO_REPLACE 0
#define RANDOM_REPLACE 1
#define REPLACE_ALG RANDOM_REPLACE
/**===========================================*/

/**============ MEMORY PARAMETERS =============*/
#define MEMORY_INPUT_FN "mem_gen/mem.dmp"
#define MEMORY_OUTPUT_FN "mem_output.dmp"
#define CACHE_OUTPUT_FN "cache.dmp"
#define MAIN_MEM_SIZE 1024
/**===========================================*/

/**============ INSTRUCTIONS PARAMETERS =============*/
#define INST_IN "inst_gen/inst.in"
#define INST_OUT "inst_gen/inst.out"
#define LOAD_INST "LD"
#define STORE_INST "ST"
/**===========================================*/

/**===========  ERROR DEFINITIONS ==================
*/
#define MEMORY_ALLOCATION   1
#define FILE_OPENNING       2
#define LOGIC_ERROR         3
#define INPUT_ERROR         4
/**
*===================================================*/

void address_split_show(mem_addr_t addr, cache_t* cache) {
    unsigned tag = addr >> (cache->n_line_bits + cache->n_offset_bits);
    unsigned line = (addr << cache->n_tag_bits) >> (cache->n_tag_bits + cache->n_offset_bits);
    unsigned offset = (addr << (cache->n_tag_bits + cache->n_line_bits)) >> (cache->n_tag_bits + cache->n_line_bits);

    printf("tag: %x\n", tag);
    printf("line: %x\n", line);
    printf("offset: %x\n", offset);
}

boolean replace(byte_t* main_mem, cache_t* cache, mem_addr_t addr) {
    boolean replaced = FALSE;

    if(REPLACE_ALG == FIFO_REPLACE) {
        replaced = fifo_replace(main_mem, cache, addr);
    }
    else if (REPLACE_ALG == RANDOM_REPLACE) {
        replaced = random_replace(main_mem, cache, addr);
    }

    return replaced;
}

int main()
{
    cache_t* cache = (cache_t*) malloc(sizeof(cache_t));
    byte_t* main_mem;
    FILE* inst_input_fp;
    FILE* inst_output_fp;
    unsigned n_line;
    size_t buffer_size = 32;
    char* line;
    char* inst;
    char* args[2];
    mem_addr_t addr;
    mem_addr_t block_addr;
    word_t* p_register = (word_t*) malloc(sizeof(word_t));
    int it;


    set_cache_params(cache, CACHE_SIZE, BLOCK_SIZE, NUMBER_OF_SETS);
    init_cache(cache);

    printf("word size: %lu bits\n", sizeof(word_t)*8);
    printf("byte size: %lu bits\n", sizeof(byte_t)*8);
    printf("mem_addr_t size: %lu bits\n", sizeof(mem_addr_t)*8);
    printf("boolean size: %lu bits\n", sizeof(boolean)*8);
    printf("tag size: %u bits\n", cache->n_tag_bits);
    printf("block size: %u bits\n", cache->n_line_bits);
    printf("offset size: %u bits\n", cache->n_offset_bits);


    main_mem = mem_load(MEMORY_INPUT_FN, MAIN_MEM_SIZE);



    if( (line =(char *)malloc(buffer_size * sizeof(char))) == NULL)
    {
        perror("Error: ");
        exit(MEMORY_ALLOCATION);
    }

    if ( (inst_input_fp = fopen(INST_IN, "r")) == NULL) {
        printf("Unable to open file %s!\n", INST_IN);
        exit(FILE_OPENNING);
    }

    if((inst_output_fp = fopen(INST_OUT, "wb")) == NULL)
    {
        printf("Unable to open file %s!\n", INST_OUT);
        exit(FILE_OPENNING);
    }


    n_line = 0;
    while ((getline(&line, &buffer_size, inst_input_fp)) != -1) {
        printf("%s", line);
        fprintf(inst_output_fp, "%s", line);
        inst = strtok (line, " ");
        it = 0;
        while ( (args[it] = strtok (NULL, " ")) != NULL) {
            it++;
        }
        if(strcmp(inst, LOAD_INST) == 0) {
            if(it == 1) {
                addr = strtol(args[0], NULL ,16);
                if(load(cache, addr,p_register)) {
                    fprintf(inst_output_fp, "--> Read value %.8x from address %.8x\n", *((unsigned*) p_register), addr);
                    printf("--> Read value %.8x from address %.8x\n", *((unsigned*) p_register), addr);
                }
                else {
                    block_addr = addr;
                    if(find_block(cache, &block_addr, MAIN_MEM_SIZE)) {
                        if(!replace(main_mem, cache, block_addr)) {
                            printf("Could not replace block in address %.8x at inst %s on line %d\n", block_addr, inst, n_line);
                            exit(LOGIC_ERROR);
                        }
                        else {
                            if(load(cache, addr,p_register)){
                                fprintf(inst_output_fp, "--> Read value %.8x from address %.8x\n", *((unsigned*) p_register), addr);
                                printf("--> Read value %x from address %.8x\n",*((unsigned*) p_register), addr);
                            }
                            else{
                                printf("Could not read address %.8x from cache for unknown reason at inst %s line %d\n", addr, inst, n_line);
                                exit(LOGIC_ERROR);
                            }
                        }
                    }
                    else {
                        printf("Address: %.8x not in memory range\n", addr);
                        exit(LOGIC_ERROR);
                    }
                }

            }
            else {
                printf("Wrong number of arguments on instruction %s on line %d", LOAD_INST, n_line);
                exit(LOGIC_ERROR);
            }
        }
        else if (strcmp(inst, STORE_INST) == 0) {
            if(it == 2) {
                addr = strtol(args[0], NULL ,16);
                *((unsigned*) p_register) = strtol(args[1], NULL ,16);
                if(store(cache, addr,p_register)) {
                    fprintf(inst_output_fp, "--> Written value %.8x on address %.8x\n", *((unsigned*) p_register), addr);
                    printf("--> Written value %.8x on address %.8x\n", *((unsigned*) p_register), addr);
                }
                else {
                    block_addr = addr;
                    if(find_block(cache, &block_addr, MAIN_MEM_SIZE)) {
                        if(!replace(main_mem, cache, block_addr)) {
                            printf("Could not replace block in address %x at inst %s line %d\n", block_addr, inst, n_line);
                            exit(LOGIC_ERROR);
                        }
                        else {
                            if(store(cache, addr,p_register)) {
                                fprintf(inst_output_fp, "--> Written value %.8x on address %.8x\n", *((unsigned*) p_register), addr);
                                printf("--> Written value %.8x on address %.8x\n", *((unsigned*) p_register), addr);
                            }
                            else{
                                printf("Could not read address %.8x from cache for unknown reason at inst %s line %d\n", addr, inst, n_line);
                                exit(LOGIC_ERROR);
                            }
                        }
                    }
                    else {
                        printf("Address: %.8x not in memory range\n", addr);
                        exit(LOGIC_ERROR);
                    }
                }
            }
            else {
                printf("Wrong number of arguments on instruction %s on line %d\n", STORE_INST, n_line);
                exit(LOGIC_ERROR);
            }
        }
        else {
            printf("Instruction %s not implemented on line %d\n", inst, n_line);
            exit(LOGIC_ERROR);
        }
        n_line++;
    }
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
