#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
/**============ MEMORY PARAMETERS =============*/
#define MEMORY_INPUT_FILE "../mem_files/ram.dmp"
#define MEMORY_OUTPUT_FILE "../mem_files/ram_output.dmp"
#define MEMORY_OUTPUT_FILE_MIF "../mem_files/ram_output.mif"
#define DEPTH 4096
#define WIDTH 32
/**===========================================*/

typedef enum radix {
    UNS,
    HEX,
    BIN
} radix;

#define ADDRESS_RADIX HEX // UNS : 0; HEX : 1; BIN : 2;
#define DATA_RADIX HEX  //  UNS : 0; HEX : 1; BIN : 2;

typedef unsigned int mem_addr_t;
typedef unsigned char byte_t;
typedef uint32_t word_t;

typedef enum boolean {
    FALSE,
    TRUE
} boolean;

word_t* mem_load();

void mem_dump_file (word_t*);

void mem_dump_stdout (word_t*);

int gen_sim_ram(word_t*);


#endif // MEMORY_H_INCLUDED
