#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#define TRUE 1
#define FALSE 0

typedef unsigned int mem_addr_t;
typedef unsigned char byte_t;
typedef struct word {
    byte_t bytes[4] ;
} word_t;
typedef char boolean;

byte_t* mem_load(const char* , const unsigned );

void mem_dump_file (byte_t* , const char* filename, const unsigned );

void mem_dump_stdout (byte_t* , const unsigned );


#endif // MEMORY_H_INCLUDED
