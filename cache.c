#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cache.h"

void set_cache_params(cache_t* cache, unsigned cache_size, unsigned block_size,  unsigned n_sets) {

    unsigned n_lines_per_set = cache_size/(block_size*n_sets);
    unsigned n_line_bits = (unsigned) sqrt( (double) n_lines_per_set);
    unsigned n_offset_bits = (unsigned) sqrt( (double) block_size);
    unsigned n_tag_bits = sizeof(mem_addr_t) - n_line_bits - n_offset_bits;

    cache->block_size = block_size;
    cache->cache_size = cache_size;
    cache->n_sets = n_sets;
    cache->n_access = 0;
    cache->n_hits = 0;
    cache->n_lines_per_set = n_lines_per_set;
    cache->n_offset_bits = n_offset_bits;
    cache->n_line_bits = n_line_bits;
    cache->n_tag_bits = n_tag_bits;
}

void init_cache(cache_t* cache) {

    cache_t* prev

    cache->c_line_head; = (c_data_node_t**) malloc(sizeof(c_data_node_t*)*cache->n_lines_per_set);
    if(!cache->c_line_head) {
        printf("Memory allocation error on: init_cache \n");
        exit(1);
    }

    for(unsigned i = 0; i < cache->n_lines_per_set; i++) {
        cache->c_line_head[i] = NULL;
    }
}

c_data_node_t* alloc_cdn (mem_addr_t addr, const byte_t* data, int block_size) {

    c_data_node_t* c_data_node;
    unsigned tag = (addr >> cache->n_line_bits) >> cache->n_offset_bits;
    unsigned line = (addr << cache->n_tag_bits) >> (cache->n_tag_bits + cache->n_offset_bits);

    c_data_node = (c_data_node_t*) malloc(sizeof(c_data_node_t));
    c_data_node->cdn_tag = tag;
    c_data_node->cdn_valid = FALSE;
    c_data_node->next = NULL;
    c_data_node->prev = NULL;
    memcpy(c_data_node->cdn_data, data, block_size);

}



boolean load (cache_t* cache, mem_addr_t addr, word_t* data_conteiner) {
    unsigned n_sets = cache->number_of_sets;

    unsigned tag = (addr >> cache->block_bits) >> cache->offset_bits;
    unsigned block = (addr << cache->tag_bits) >> (cache->tag_bits + cache->offset_bits);
    unsigned offset = (addr << (cache->tag_bits + cache->block_bits)) >> (cache->tag_bits +cache->block_bits) ;
    boolean found = FALSE;
    unsigned count_set;

    cache->access++;
    for(count_set = 0; count_set < n_sets; count_set++) {

        if(cache->c_set[count_set][block].cl_tag == tag &&
            cache->c_set[count_set][block].cl_valid == TRUE) {
            found = TRUE;
            *data_conteiner = cache->c_set[count_set][block].cl_block[offset];
        }
        else {

        }
    }
    return found;
}

boolean store (cache_t* cache, mem_addr_t addr, word_t data) {
    unsigned n_sets = cache->number_of_sets;

    unsigned tag = (addr >> cache->block_bits) >> cache->offset_bits;
    unsigned block = (addr << cache->tag_bits) >> (cache->tag_bits + cache->offset_bits);
    unsigned offset = (addr << (cache->tag_bits + cache->block_bits)) >> (cache->tag_bits +cache->block_bits) ;
    boolean found = FALSE;
    unsigned count_set;

    cache->access++;
    for(count_set = 0; count_set < n_sets; count_set++) {

        if(cache->c_set[count_set][block].cl_tag == tag &&
            cache->c_set[count_set][block].cl_valid == TRUE) {
            found = TRUE;
            cache->c_set[count_set][block].cl_block[offset] = data;
        }
    }
    return found;
}

void cache_dump_file (cache_t* cache) {
    FILE *ptr_fp;

    unsigned n_sets = cache->number_of_sets;
    unsigned n_blocks = cache->blocks_per_set;
    unsigned block_size = cache->block_size;
    char filename[32];

    for(unsigned i = 0; i < n_sets; i++) {
        sprintf(filename, "cache_set%u.dmp", i);
        if((ptr_fp = fopen(filename, "wb")) == NULL)
        {
            printf("Unable to open file %s!\n", filename);
            exit(1);
        }

        for(unsigned j = 0; j < n_blocks; j++) {
            if( fwrite(cache->c_set[i][j].cl_block, block_size*sizeof(byte_t), 1, ptr_fp) != 1)
            {
                printf("Write cache error in:!\n");
                printf("\tset %d and block %d\n", i,j);
                exit(1);
            }
            if( fwrite(&cache->c_set[i][j].cl_tag, sizeof(mem_addr_t), 1, ptr_fp) != 1)
            {
                printf("Write cache tag error in:!\n");
                printf("\tset %d and block %d\n", i,j);
                exit(1);
            }
            if( fwrite(&cache->c_set[i][j].cl_valid, sizeof(boolean), 1, ptr_fp) != 1)
            {
                printf("Write cache valid bit error in:!\n");
                printf("\tset %d and block %d\n", i,j);
                exit(1);
            }
        }
        fclose(ptr_fp);
    }
}
