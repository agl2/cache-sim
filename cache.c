#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "cache.h"

void set_cache_params(cache_t* cache, unsigned cache_size, unsigned block_size,  unsigned n_sets) {

    unsigned n_lines_per_set = cache_size/(block_size*n_sets);
    unsigned n_line_bits = (unsigned) sqrt( (double) n_lines_per_set);
    unsigned n_offset_bits = (unsigned) sqrt( (double) block_size);
    unsigned n_tag_bits = sizeof(mem_addr_t) - n_line_bits - n_offset_bits;

    cache->block_size = block_size;
    cache->cache_size = cache_size;
    cache->n_sets = n_sets;
    cache->write_count = 0;
    cache->read_count = 0;
    cache->read_hit_count = 0;
    cache->write_hit_count = 0;
    cache->n_lines_per_set = n_lines_per_set;
    cache->n_offset_bits = n_offset_bits;
    cache->n_line_bits = n_line_bits;
    cache->n_tag_bits = n_tag_bits;
}

void init_cache(cache_t* cache) {

    cache->c_line_head = (c_data_block_t**) malloc(sizeof(c_data_block_t*)*cache->n_lines_per_set);

    if(!cache->c_line_head) {
        printf("Memory allocation error on: init_cache \n");
        exit(1);
    }

    for(unsigned i = 0; i < cache->n_lines_per_set; i++) {
        cache->c_line_head[i] = NULL;
    }
}

c_data_block_t* alloc_cdb (cache_t* cache, mem_addr_t addr, const byte_t* data, unsigned set_number) {

    c_data_block_t* c_data_node;
    unsigned tag = (addr >> cache->n_line_bits) >> cache->n_offset_bits;

    c_data_node = (c_data_block_t*) malloc(sizeof(c_data_block_t));
    c_data_node->cdb_tag = tag;
    c_data_node->cdb_valid = FALSE;
    c_data_node->next = NULL;
    c_data_node->prev = NULL;
    c_data_node->set_number = set_number;
    memcpy(c_data_node->cdb_data, data, cache->block_size);

    return c_data_node;
}

boolean load (cache_t* cache, mem_addr_t addr, word_t* data_conteiner) {

    unsigned tag = (addr >> cache->n_line_bits) >> cache->n_offset_bits;
    unsigned line = (addr << cache->n_tag_bits) >> (cache->n_tag_bits + cache->n_offset_bits);
    unsigned offset = (addr << (cache->n_tag_bits + cache->n_line_bits)) >> (cache->n_tag_bits +cache->n_line_bits) ;
    boolean found = FALSE;

    cache->read_count++;
    c_data_block_t *cdb_ptr = cache->c_line_head[line];

    while(cdb_ptr != NULL) {
        if(cdb_ptr->cdb_valid && cdb_ptr->cdb_tag == tag) {
            found = TRUE;
            cache->read_hit_count++;
            *data_conteiner = cdb_ptr->cdb_data[offset];
        }
        cdb_ptr = cdb_ptr->next;
    }
    return found;
}

boolean store (cache_t* cache, mem_addr_t addr, word_t data) {

    unsigned tag = (addr >> cache->n_line_bits) >> cache->n_offset_bits;
    unsigned line = (addr << cache->n_tag_bits) >> (cache->n_tag_bits + cache->n_offset_bits);
    unsigned offset = (addr << (cache->n_tag_bits + cache->n_line_bits)) >> (cache->n_tag_bits +cache->n_line_bits) ;
    boolean found = FALSE;

    cache->write_count++;
    c_data_block_t *cdb_ptr = cache->c_line_head[line];

    while(cdb_ptr != NULL) {
        if(cdb_ptr->cdb_valid && cdb_ptr->cdb_tag == tag) {
            found = TRUE;
            cache->write_hit_count++;
            cdb_ptr->cdb_data[offset] = data;
        }
        cdb_ptr = cdb_ptr->next;
    }

    return found;
}

void cache_dump_file (cache_t* cache) {
    /*FILE *ptr_fp;

    unsigned n_sets = cache->number_of_sets;
    unsigned n_lines = cache->lines_per_set;
    unsigned block_size = cache->block_size;
    char filename[32];

    for(unsigned i = 0; i < n_sets; i++) {
        sprintf(filename, "cache_set%u.dmp", i);
        if((ptr_fp = fopen(filename, "wb")) == NULL)
        {
            printf("Unable to open file %s!\n", filename);
            exit(1);
        }

        for(unsigned j = 0; j < n_lines; j++) {
            if( fwrite(cache->c_set[i][j].cl_line, block_size*sizeof(byte_t), 1, ptr_fp) != 1)
            {
                printf("Write cache error in:!\n");
                printf("\tset %d and line %d\n", i,j);
                exit(1);
            }
            if( fwrite(&cache->c_set[i][j].cl_tag, sizeof(mem_addr_t), 1, ptr_fp) != 1)
            {
                printf("Write cache tag error in:!\n");
                printf("\tset %d and line %d\n", i,j);
                exit(1);
            }
            if( fwrite(&cache->c_set[i][j].cl_valid, sizeof(boolean), 1, ptr_fp) != 1)
            {
                printf("Write cache valid bit error in:!\n");
                printf("\tset %d and line %d\n", i,j);
                exit(1);
            }
        }
        fclose(ptr_fp);
    }*/
}
