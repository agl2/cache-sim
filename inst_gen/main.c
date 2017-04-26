#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define N_INST 32
#define LOAD_TYPE 0
#define STORE_TYPE 1
#define LOAD_INST "LOAD"
#define STORE_INST "STORE"
#define INST_FILENAME "32inst.ins"

int main()
{
    srand(time(NULL));   // should only be called once
    FILE *ptr_fp;
    int inst_type;
    int address;
    int value;
    char filename[20] = "32inst.ins";

    if((ptr_fp = fopen(filename, "wb")) == NULL) {
        printf("Unable to open file %s!\n", filename);
        exit(1);
    }

    for(int i; i < N_INST;  i++) {
        inst_type = rand()%2;
        address = rand();
        value = rand();

        if(inst_type == LOAD_TYPE) {
            if( fprintf(ptr_fp, "%s %x\n", LOAD_INST, address) <0) {
                printf("Error writing file line %d\n", i);
                exit(1);
            }
        }else {
            if( fprintf(ptr_fp, "%s %x, %x\n", STORE_INST, address, value) <0) {
                printf("Error writing file line %d\n", i);
                exit(1);
            }
        }
    }


    printf("Generation Successful!\n");
    return 0;
}
