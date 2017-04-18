#ifndef CACHE_H_INCLUDED
#define CACHE_H_INCLUDED
#include "memory.h"

typedef struct c_data_node {
    boolean cdn_valid;
    mem_addr_t cdn_tag;
    byte_t* cdn_data;

    struct cache_line* prev;
    struct cache_line* next;
} c_data_node_t;

typedef struct cache {
    c_data_node_t** c_line_head;

    unsigned cache_size;
    unsigned block_size;
    unsigned n_sets;
    unsigned n_lines_per_set;
    unsigned n_tag_bits;
    unsigned n_line_bits;
    unsigned n_offset_bits;
    unsigned n_access;
    unsigned n_hits;
} cache_t;

void set_cache_params(cache_t*, unsigned, unsigned, unsigned);
void init_cache(cache_t*);

boolean load (cache_t*, mem_addr_t, word_t*);
boolean store (cache_t*, mem_addr_t, word_t);

word_t lru_replace(cache_t*, byte_t*, mem_addr_t);

void cache_dump_file (cache_t* );

#endif // CACHE_H_INCLUDED
