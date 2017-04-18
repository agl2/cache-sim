#include "memory.h"

byte_t* mem_load(const char* filename, const unsigned mem_size) {
    FILE* ptr_fp;
    byte_t* mem;

    /* Part E */
    mem = (byte_t*)malloc(mem_size * sizeof(byte_t));
    if(!mem)
    {
        printf("Memory allocation error!\n");
        exit(1);
    }

    /* Part F */
    if((ptr_fp = fopen(filename, "rb"))==NULL)
    {
        printf("Unable to open the file %s!\n", filename);
        exit(1);
    }

    /* Part G */
    if(fread(mem, mem_size * sizeof( byte_t ), 1, ptr_fp) != 1)
    {
        printf( "Read error!\n" );
        exit( 1 );
    }
    fclose(ptr_fp);

    return mem;
}

void mem_dump_file (byte_t* mem, const char* filename, const unsigned mem_size) {
    FILE *ptr_fp;
    /* Part C */
    if((ptr_fp = fopen(filename, "wb")) == NULL)
    {
        printf("Unable to open file %s!\n", filename);
        exit(1);
    }

    /* Part D */
    if( fwrite(mem, mem_size*sizeof(byte_t), 1, ptr_fp) != 1)
    {
        printf("Write error!\n");
        exit(1);
    }
    fclose(ptr_fp);
}

void mem_dump_stdout (byte_t* mem, const unsigned mem_size) {
    unsigned counter;
    /* Part H */
    printf("Mem dump:\n");
    for(counter = 0; counter < mem_size; counter++)
        printf("%x: %d\n", counter*4, mem[counter]);
}
