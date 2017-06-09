#include "memory.h"

word_t* mem_load() {
    FILE* ptr_fp;
    word_t* mem;

    /* Part E */
    mem = (word_t*)malloc(DEPTH * sizeof(word_t));
    if(!mem)
    {
        printf("Memory allocation error!\n");
        exit(1);
    }

    /* Part F */
    if((ptr_fp = fopen(MEMORY_INPUT_FILE, "rb"))==NULL)
    {
        printf("Unable to open the file %s!\n", MEMORY_INPUT_FILE);
        exit(1);
    }

    /* Part G */
    if(fread(mem, DEPTH * sizeof( word_t ), 1, ptr_fp) != 1)
    {
        printf( "Memory read error!\n" );
        exit( 1 );
    }
    fclose(ptr_fp);

    return mem;
}

void mem_dump_file (word_t* mem) {
    FILE *ptr_fp;
    /* Part C */
    if((ptr_fp = fopen(MEMORY_OUTPUT_FILE, "wb")) == NULL)
    {
        printf("Unable to open file %s!\n", MEMORY_OUTPUT_FILE);
        exit(1);
    }

    /* Part D */
    if( fwrite(mem, DEPTH*sizeof(word_t), 1, ptr_fp) != 1)
    {
        printf("Write error!\n");
        exit(1);
    }
    fclose(ptr_fp);
}

void mem_dump_stdout (word_t* mem) {
    unsigned counter;
    /* Part H */
    printf("Mem dump:\n");
    for(counter = 0; counter < DEPTH; counter++)
        printf("%x: %d\n", counter*4, mem[counter]);
}

int gen_sim_ram(word_t *data_array)
{

    srand(time(NULL));   // should only be called once
    word_t addr;
    FILE *ptr_fp;
    char buffer [32];

    printf("\nsize of word_t: %d\n", sizeof(word_t));


    /* Part C */
    if((ptr_fp = fopen(MEMORY_OUTPUT_FILE_MIF, "wb")) == NULL)
    {
        printf("Unable to open file!\n");
        exit(2);
    }else printf("Opened file successfully for writing.\n");

    fprintf(ptr_fp, "WIDTH=%u;\n", WIDTH);
    fprintf(ptr_fp, "DEPTH=%u;\n", DEPTH);
    fprintf(ptr_fp, "ADDRESS_RADIX=%s;\n", ADDRESS_RADIX == UNS ? "UNS" : ADDRESS_RADIX == HEX ? "HEX" : "BIN");
    fprintf(ptr_fp, "DATA_RADIX=%s;\n", DATA_RADIX == UNS ? "UNS" : DATA_RADIX == HEX ? "HEX" : "BIN");
    fprintf(ptr_fp, "CONTENT BEGIN\n\n");

    for(addr = 0; addr < DEPTH; addr++) {
         word_t data = data_array[addr];

         switch(ADDRESS_RADIX) {

            case UNS:
                fprintf(ptr_fp, "%u:", addr);
                break;

            case HEX:
                fprintf(ptr_fp, "%x:", addr);
                break;

            case BIN:
                itoa (addr,buffer,2);
                fprintf(ptr_fp, "%s:", buffer);
                break;
            default:
                printf("ADDRESS_RADIX not valid");
                exit(1);

         }

        switch(DATA_RADIX) {

            case UNS:
                fprintf(ptr_fp, "%u;\n", data);
                break;

            case HEX:
                fprintf(ptr_fp, "%x;\n", data);
                break;

            case BIN:
                itoa (data,buffer,2);
                fprintf(ptr_fp, "%s\n", buffer);
                break;
            default:
                printf("DATA_RADIX not valid");
                exit(1);

         }
    }

    fprintf(ptr_fp, "\nEND;");


    fclose(ptr_fp);

    return 0;
}
