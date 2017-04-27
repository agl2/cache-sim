#ifndef CACHE_H_INCLUDED
#define CACHE_H_INCLUDED
#include "memory.h"

typedef struct c_data_block {
    boolean cdb_valid;
    mem_addr_t cdb_tag;
    byte_t* cdb_data;
    unsigned set_number;

    struct c_data_block* prev;
    struct c_data_block* next;
} c_data_block_t;

typedef struct cache {
    c_data_block_t** c_line_head;

    unsigned cache_size;
    unsigned block_size;
    unsigned n_sets;
    unsigned n_lines_per_set;
    unsigned n_tag_bits;
    unsigned n_line_bits;
    unsigned n_offset_bits;

    unsigned read_count;
    unsigned write_count;
    unsigned write_hit_count;
    unsigned read_hit_count;
} cache_t;

void set_cache_params(cache_t*, unsigned, unsigned, unsigned);
void init_cache(cache_t*);

boolean load (cache_t*, mem_addr_t, word_t*);

boolean store (cache_t*, mem_addr_t, word_t);

boolean random_replace(byte_t* main_mem, cache_t* cache, mem_addr_t addr);

void lru_replace(byte_t* , cache_t* , mem_addr_t );

boolean fifo_replace(byte_t* , cache_t* , mem_addr_t );

void cache_dump_file (cache_t* );

boolean find_block(cache_t* , mem_addr_t*, unsigned);

#endif // CACHE_H_INCLUDED
