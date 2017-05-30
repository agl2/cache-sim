#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define LOAD_TYPE 0
#define STORE_TYPE 1
#define LOAD_INST "LD"
#define STORE_INST "ST"
#define INST_FILENAME "inst.in"
#define N_INST 128
#define DEPTH 4096

int main()
{
    srand(time(NULL));   // should only be called once
    FILE *ptr_fp;
    int inst_type;
    int address;
    int value;

    if((ptr_fp = fopen(INST_FILENAME, "wb")) == NULL) {
        printf("Unable to open file %s!\n", INST_FILENAME);
        exit(1);
    }

    for(int i = 0; i < N_INST;  i++) {
        inst_type = rand()%2;
        //inst_type = STORE_TYPE;
        address = 4*(rand()%(DEPTH/4));
        value = rand();

        if(inst_type == LOAD_TYPE) {
            if( fprintf(ptr_fp, "%s %.8x\n", LOAD_INST, address) <0) {
                printf("Error writing file line %d\n", i);
                exit(1);
            }
        }else {
            if( fprintf(ptr_fp, "%s %.8x %.8x\n", STORE_INST, address, value) <0) {
                printf("Error writing file line %d\n", i);
                exit(1);
            }
        }
    }


    printf("Generation Successful!\n");
    return 0;
}
