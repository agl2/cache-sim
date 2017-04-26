#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MEM_SIZE_BYTES 1024

int main(void)
{

    srand(time(NULL));   // should only be called once
    int counter;
    int *ptr_d;
    FILE *ptr_fp;
    int mem_size_words = MEM_SIZE_BYTES/sizeof(int);

    /* Part A */
    ptr_d = (int *)malloc(mem_size_words * sizeof(int));
    if(!ptr_d)
    {
        printf("Memory allocation error!\n");
        exit(1);
    }else printf("Memory allocation successful.\n");

    /* Part B */
    for(counter = 0; counter < mem_size_words; counter++)
        ptr_d[counter] = (int) rand();

    /* Part C */
    if((ptr_fp = fopen("mem.dmp", "wb")) == NULL)
    {
        printf("Unable to open file!\n");
        exit(1);
    }else printf("Opened file successfully for writing.\n");

    /* Part D */
    if( fwrite(ptr_d, mem_size_words*sizeof(int), 1, ptr_fp) != 1)
    {
        printf("Write error!\n");
        exit(1);
    }else printf("Write was successful.\n");
    fclose(ptr_fp);
    free(ptr_d);

    /* Part E */
    ptr_d = (int *)malloc(mem_size_words * sizeof(int));
    if(!ptr_d)
    {
        printf("Memory allocation error!\n");
        exit(1);
    }else printf("Memory allocation successful.\n");

    /* Part F */
    if((ptr_fp = fopen("mem.dmp", "rb"))==NULL)
    {
        printf("Unable to open the file!\n");
        exit(1);
    }else printf("Opened file successfully for reading.\n");

    /* Part G */
    if(fread(ptr_d, mem_size_words * sizeof( int ), 1, ptr_fp) != 1)
    {
        printf( "Read error!\n" );
        exit( 1 );
    }else printf( "Read was successful.\n" );
    fclose(ptr_fp);

    /* Part H */
    printf("The numbers read from file are:\n");
    for(counter = 0; counter < mem_size_words; counter++)
        printf("%d ", ptr_d[counter]);

    /* Part I */
    free(ptr_d);
    return 0;
}
