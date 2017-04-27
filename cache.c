#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <byteswap.h>
#include "cache.h"

void set_cache_params(cache_t* cache, unsigned cache_size, unsigned block_size,  unsigned n_sets) {

    unsigned n_lines_per_set = cache_size/(block_size*n_sets);
    unsigned n_line_bits = (unsigned) log2l( (long) n_lines_per_set);
    unsigned n_offset_bits = (unsigned) log2l ( (long) block_size);
    unsigned n_tag_bits = (unsigned )sizeof(mem_addr_t)*8 - n_line_bits - n_offset_bits;

    srand(time(NULL));   // should only be called once

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

boolean load (cache_t* cache, mem_addr_t addr, word_t* data_conteiner) {

    unsigned tag = addr >> (cache->n_line_bits + cache->n_offset_bits);
    unsigned line = (addr << cache->n_tag_bits) >> (cache->n_tag_bits + cache->n_offset_bits);
    unsigned offset = (addr << (cache->n_tag_bits + cache->n_line_bits)) >> (cache->n_tag_bits + cache->n_line_bits);
    boolean found = FALSE;

    cache->read_count++;
    c_data_block_t *cdb_ptr = cache->c_line_head[line];

    while(cdb_ptr != NULL) {
        if(cdb_ptr->cdb_valid && cdb_ptr->cdb_tag == tag) {
            found = TRUE;
            cache->read_hit_count++;
            memcpy(data_conteiner, cdb_ptr->cdb_data + offset, sizeof(word_t));
            *((int32_t*)data_conteiner) = bswap_32(*((int32_t*)data_conteiner));
        }
        cdb_ptr = cdb_ptr->next;
    }
    return found;
}

boolean store (cache_t* cache, mem_addr_t addr, word_t data) {

    unsigned tag = addr >> (cache->n_line_bits + cache->n_offset_bits);
    unsigned line = (addr << cache->n_tag_bits) >> (cache->n_tag_bits + cache->n_offset_bits);
    unsigned offset = (addr << (cache->n_tag_bits + cache->n_line_bits)) >> (cache->n_tag_bits + cache->n_line_bits);
    boolean found = FALSE;

    cache->write_count++;
    c_data_block_t *cdb_ptr = cache->c_line_head[line];

    while(cdb_ptr != NULL) {
        if(cdb_ptr->cdb_valid && cdb_ptr->cdb_tag == tag) {
            found = TRUE;
            cache->write_hit_count++;
            *((word_t*) cdb_ptr->cdb_data + offset) = data;

        }
        cdb_ptr = cdb_ptr->next;
    }

    return found;
}


/**
* @name: alloc_cdb
* @params:  cache: Pointer to instance of cache struct
*           data: Pointer to initial byte of block to be inserted
*           set_number: Number of cache set where the block will be inserted
*           tag: Tag bits to be stored
* @description: Creates a cache data block
*/
c_data_block_t* alloc_cdb (cache_t* cache, const byte_t* data, unsigned set_number, mem_addr_t tag) {

    c_data_block_t* c_data_node;

    c_data_node = (c_data_block_t*) malloc(sizeof(c_data_block_t));
    if(!c_data_node)
    {
        printf("Memory allocation error while allocating c_data_node in function alloc_cdb \n");
        exit(1);
    }
    c_data_node->cdb_data = (byte_t*) malloc(sizeof(byte_t)*cache->block_size);
    if(!c_data_node)
    {
        printf("Memory allocation error while allocating c_data_node->cdb_data in function alloc_cdb \n");
        exit(1);
    }
    c_data_node->cdb_tag = tag;
    c_data_node->cdb_valid = FALSE;
    c_data_node->next = NULL;
    c_data_node->prev = NULL;
    c_data_node->set_number = set_number;
    memcpy(c_data_node->cdb_data, data, cache->block_size);

    return c_data_node;
}

/**
* @name: random_replace
* @params:  main_mem: Pointer to main memory initial position
*           cache: Pointer to instance of cache struct
*           addr: Address of block to be inserted in cache
* @description: Inserts a block in cache replacing an older one randomly
*/
boolean random_replace(byte_t* main_mem, cache_t* cache, mem_addr_t addr)  {

    ///Split the address bits in tag and line
    unsigned tag = addr >> (cache->n_line_bits + cache->n_offset_bits);
    unsigned line = (addr << cache->n_tag_bits) >> (cache->n_tag_bits + cache->n_offset_bits);

    boolean replaced = FALSE;

    ///Create two pointers, one pointing for the head of the first list of data blocks and another buffer to store previous element on the list.
    c_data_block_t *cdb_ptr = cache->c_line_head[line];
    c_data_block_t *cdb_ptr_buff = NULL;

    ///This loop search for a blank position, if there is one, the block is inserted on it.
    for(int i = 0; i < cache->n_sets; i++) {
        if(cdb_ptr == NULL) {
            cdb_ptr = alloc_cdb (cache, main_mem + addr , i, tag);
            if(cdb_ptr_buff != NULL) {
                cdb_ptr_buff->next = cdb_ptr;
                cdb_ptr->prev = cdb_ptr_buff;
            }
            cdb_ptr->cdb_valid = TRUE;
            replaced = TRUE;
            if(i == 0) {
                cache->c_line_head[line] = cdb_ptr;
            }
        }
        else {
            cdb_ptr_buff = cdb_ptr;
            cdb_ptr = cdb_ptr->next;
        }
    }

    /**If the previous loop could not find a blank position,
    * then this block of code randomly selects a block to be replaced
    * and replaces it references on the list for the new one references
    */
    c_data_block_t *cdb_ptr_new = NULL;
    if(!replaced) {
        unsigned set_replaced = rand()%cache->n_sets;
        cdb_ptr = cache->c_line_head[line];
        while(cdb_ptr != NULL) {
            if(cdb_ptr->set_number == set_replaced) {
                cdb_ptr_new = alloc_cdb (cache, main_mem + addr , set_replaced, tag);
                cdb_ptr_new->prev = cdb_ptr->prev;
                cdb_ptr_new->next= cdb_ptr->next;
                if(cdb_ptr_new->prev != NULL)
                    cdb_ptr_new->prev->next = cdb_ptr_new;
                if(cdb_ptr_new->next != NULL)
                    cdb_ptr_new->next->prev = cdb_ptr_new;
                cdb_ptr_new->cdb_valid = TRUE;

                if(cdb_ptr == cache->c_line_head[line]) {
                    cache->c_line_head[line] = cdb_ptr_new;
                }
                free(cdb_ptr);
                cdb_ptr = NULL;
                replaced = TRUE;


            } else {
                cdb_ptr = cdb_ptr->next;
            }
        }
    }
    return replaced;
}

void lru_replace(byte_t* main_mem, cache_t* cache, mem_addr_t addr)  {

}


/**
* @name: fifo_replace
* @params:  main_mem: Pointer to main memory initial position
*           cache: Pointer to instance of cache struct
*           addr: Address of block to be inserted in cache
* @description: Inserts a block in cache replacing an older one using FIFO algorithm
*/
boolean fifo_replace(byte_t* main_mem, cache_t* cache, mem_addr_t addr) {
    ///Split the address bits in tag and line
    unsigned tag = addr >> (cache->n_line_bits + cache->n_offset_bits);
    unsigned line = (addr << cache->n_tag_bits) >> (cache->n_tag_bits + cache->n_offset_bits);

    boolean replaced = FALSE;

    ///Create two pointers, one pointing for the head of the first list of data blocks and another buffer to store previous element on the list.
    c_data_block_t *cdb_ptr = cache->c_line_head[line];
    c_data_block_t *cdb_ptr_buff = NULL;

    ///This loop search for a blank position, if there is one, the block is inserted on it.
    for(int i = 0; i < cache->n_sets; i++) {
        if(cdb_ptr == NULL) {
            cdb_ptr = alloc_cdb (cache, main_mem + addr , i, tag);
            if(cdb_ptr_buff != NULL) {
                cdb_ptr->next = cdb_ptr_buff;
                cdb_ptr_buff->prev = cdb_ptr;
            }
            cdb_ptr->cdb_valid = TRUE;
            replaced = TRUE;
            cache->c_line_head[line] = cdb_ptr;
        }
        else {
            cdb_ptr_buff = cdb_ptr;
            cdb_ptr = cdb_ptr->next;
        }
    }

    /**If the previous loop could not find a blank position,
    * then this block of code selects the older data block in the cache line
    * and replaces it references on the list for the new one references
    */
    if(!replaced) {
        cdb_ptr_buff = cache->c_line_head[line];
        ///Gets the last element from the list
        while(cdb_ptr_buff->next != NULL) {
                cdb_ptr_buff = cdb_ptr_buff->next;
        }
        cdb_ptr = alloc_cdb (cache, main_mem + addr , cdb_ptr_buff->set_number, tag);
        cdb_ptr->next = cache->c_line_head[line];
        cache->c_line_head[line]->prev = cdb_ptr;
        cache->c_line_head[line] = cdb_ptr;
        cdb_ptr_buff->prev->next = NULL;
        replaced = TRUE;
        free(cdb_ptr_buff);
    }
    return replaced;
}

void cache_dump_file (cache_t* cache) {
    FILE *ptr_fp;

    unsigned n_sets = cache->n_sets;
    unsigned n_lines = cache->n_lines_per_set;
    unsigned block_size = cache->block_size;

    char filename[20];
    strcpy(filename, "cache.dmp");
    if((ptr_fp = fopen(filename, "wb")) == NULL)
    {
        printf("Unable to open file %s!\n", filename);
        exit(1);
    }

    c_data_block_t *cdb_ptr = NULL;
    for(unsigned i = 0; i < n_sets; i++) {

        if( fprintf(ptr_fp, "Set %u:\n", i) <0) {
            printf("Write cache file dump error in:");
            printf("\tset %d\n", i);
            exit(1);
        }

        for(unsigned j = 0; j < n_lines; j++) {
            if( fprintf(ptr_fp, "\tLine %u:\n", j) < 0) {
                printf("Write cache file dump error in:");
                printf("\tset %d line %d\n", i, j);
                exit(1);
            }
            cdb_ptr = cache->c_line_head[j];
            while(cdb_ptr != NULL) {
                if(cdb_ptr->set_number == i) {
                    if( fprintf(ptr_fp, "\t\tData: ") < 0) {
                        printf("Write cache file dump error in:");
                        printf("\tset %d line %d\n", i, j);
                        exit(1);
                    }
                    for(unsigned k = 0; k < block_size; k++) {

                        if( fprintf(ptr_fp, "%.2x", cdb_ptr->cdb_data[k] ) < 0) {
                            printf("Write cache file dump error in:");
                            printf("\tset %d line %d byte %d\n", i, j, k);
                            exit(1);
                        }
                    }
                    if( fprintf(ptr_fp, "\n\t\tTag: %.8x\n", cdb_ptr->cdb_tag ) < 0) {
                        printf("Write cache file dump error in:!\n");
                        printf("\tset %d line %d\n", i, j);
                        exit(1);
                    }
                }
                cdb_ptr = cdb_ptr->next;
            }
        }
    }
    fclose(ptr_fp);
}


boolean find_block(cache_t* cache, mem_addr_t *addr, unsigned mem_size) {

    for(mem_addr_t block_addr = 0; block_addr < mem_size; block_addr += cache->block_size ) {
        if(*addr >= block_addr && *addr < block_addr + cache->block_size) {
            *addr = block_addr;
            return TRUE;
        }
    }
    return FALSE;
}
