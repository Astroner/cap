#include <stdio.h>
#include <string.h>

#include "./cap.h"

int main(int argc, char** argv) {
    Cap_Iterator args;
    Cap_Init(argc, argv, &args);

    Cap_Item arg;
    while(Cap_Next(&args, &arg)) {
        switch(arg.type) {
            case CAP_ARG:
                printf("Arg\n");
                break;

            case CAP_FLAG:
                printf("Flag\n");
                break;

            case CAP_LONG_FLAG:
                printf("Long flag\n");
                break;
        }
    }

    return 0;
}