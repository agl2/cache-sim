#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cache.h"
#include "memory.h"

/**============ INSTRUCTIONS PARAMETERS =============*/
#define INST_IN "../inst_gen/inst.in"
#define INST_OUT "inst.out"
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

boolean replace(word_t* main_mem, cache_t* cache, mem_addr_t addr) {
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
    word_t* main_mem;
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

    printf("word size: %u bits\n", sizeof(word_t)*8);
    printf("byte size: %u bits\n", sizeof(byte_t)*8);
    printf("mem_addr_t size: %u bits\n", sizeof(mem_addr_t)*8);
    printf("boolean size: %u bits\n", sizeof(boolean)*8);
    printf("tag size: %u bits\n", cache->n_tag_bits);
    printf("block size: %u bits\n", cache->n_line_bits);
    printf("offset size: %u bits\n", cache->n_offset_bits);


    main_mem = mem_load();



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


    n_line = 1;
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
                addr = addr >> 2;
                if(load(cache, addr,p_register)) {
                    fprintf(inst_output_fp, "--> Read value %.8x from address %.8x\n", *((unsigned*) p_register), addr << 2);
                    printf("--> Read value %.8x from address %.8x\n", *((unsigned*) p_register), addr << 2);
                }
                else {
                    block_addr = addr;
                    if(find_block(cache, &block_addr, DEPTH)) {
                        if(!replace(main_mem, cache, block_addr)) {
                            printf("Could not replace block in address %.8x at inst %s on line %d\n", block_addr, inst, n_line);
                            exit(LOGIC_ERROR);
                        }
                        else {
                            if(load(cache, addr,p_register)){
                                fprintf(inst_output_fp, "--> Read value %.8x from address %.8x\n", *((unsigned*) p_register), addr << 2);
                                printf("--> Read value %x from address %.8x\n",*((unsigned*) p_register), addr << 2);
                            }
                            else{
                                printf("Could not read address %.8x from cache for unknown reason at inst %s line %d\n", addr << 2, inst, n_line);
                                exit(LOGIC_ERROR);
                            }
                        }
                    }
                    else {
                        printf("Address: %.8x not in memory range\n", addr << 2);
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
                addr = addr >> 2;
                *((unsigned*) p_register) = strtol(args[1], NULL ,16);
                if(store(cache, addr,p_register)) {
                    fprintf(inst_output_fp, "--> Written value %.8x on address %.8x\n", *((unsigned*) p_register), addr << 2);
                    printf("--> Written value %.8x on address %.8x\n", *((unsigned*) p_register), addr << 2);
                }
                else {
                    block_addr = addr;
                    if(find_block(cache, &block_addr, DEPTH)) {
                        if(!replace(main_mem, cache, block_addr)) {
                            printf("Could not replace block in address %x at inst %s line %d\n", block_addr, inst, n_line);
                            exit(LOGIC_ERROR);
                        }
                        else {
                            if(store(cache, addr,p_register)) {
                                fprintf(inst_output_fp, "--> Written value %.8x on address %.8x\n", *((unsigned*) p_register), addr << 2);
                                printf("--> Written value %.8x on address %.8x\n", *((unsigned*) p_register), addr << 2);
                            }
                            else{
                                printf("Could not write on address %.8x from cache for unknown reason at inst %s line %d\n", addr << 2, inst, n_line);
                                exit(LOGIC_ERROR);
                            }
                        }
                    }
                    else {
                        printf("Address: %.8x not in memory range\n", addr << 2);
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
        mem_addr_t addr = (mem_addr_t) rand()%DEPTH;
        if(!find_block(cache, &addr, DEPTH)) {
            printf("Address: %.8x not in memory range", addr);
        }
        random_replace(main_mem, cache, addr);
    }*/

    /*for(int i = 0; i < 64; i++) {
        printf("Fifo Replace: %d\n", i);
        mem_addr_t addr = (mem_addr_t) rand()%DEPTH;
        if(!find_block(cache, &addr, DEPTH)) {
            printf("Address: %.8x not in memory range", addr);
            exit(3);
        }
        fifo_replace(main_mem, cache, addr);
    }*/

    cache_dump_file(cache);
    copy_back_all(cache, main_mem);
    mem_dump_file(main_mem);
    gen_sim_ram((unsigned*) main_mem);
    return 0;
}
