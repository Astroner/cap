#include "tests-new.h"

#include "tests.h"

#define CAP_IMPLEMENTATION
#include "../cap.h"

DESCRIBE(main) {
    IT("reads arguments correctly") {
        char* argv[] = { "arg1", "arg2", "-dfc=val", "-p", "arg3", "-b", "arg4", "--flag", "--str=val", "arg5" };
        int argc = sizeof(argv) / sizeof(argv[0]);

        Cap_Iterator argsIterator;
        Cap_Init(argc, argv, &argsIterator);

        struct {
            int hasArg1;
            int hasArg2;
            int hasArg3;
            int hasArg4;
            int hasArg5;

            int hasD;
            int hasF;
            int hasC;
            int hasP;
            int hasB;

            int hasFlag;
            int hasStr;
        } results = {0};

        Cap_Item arg;
        while(Cap_Next(&argsIterator, &arg)) {
            switch(arg.type) {
                case CAP_FLAG:
                    switch(arg.value.flag.ch) {
                        case 'd':
                            results.hasD = 1;
                            EXPECT(arg.value.flag.attached) TO_BE_NULL;

                            break;

                        case 'f':
                            results.hasF = 1;
                            EXPECT(arg.value.flag.attached) TO_BE_NULL;

                            break;

                        case 'c':
                            results.hasC = 1;
                            EXPECT(arg.value.flag.attached) TO_BE_STRING("val");

                            break;

                        case 'p': 
                            results.hasP = 1;
                            EXPECT(arg.value.flag.attached) TO_BE_NULL;

                            break;

                        case 'b':
                            results.hasB = 1;
                            EXPECT(arg.value.flag.attached) TO_BE_NULL;

                            break;
                    }
                    break;
                
                case CAP_ARG:
                    if(strcmp(arg.value.arg, "arg1") == 0) {
                        results.hasArg1 = 1;
                    } else if(strcmp(arg.value.arg, "arg2") == 0) {
                        results.hasArg2 = 1;
                    } else if(strcmp(arg.value.arg, "arg3") == 0) {
                        results.hasArg3 = 1;
                    } else if(strcmp(arg.value.arg, "arg4") == 0) {
                        results.hasArg4 = 1;
                    } else if(strcmp(arg.value.arg, "arg5") == 0) {
                        results.hasArg5 = 1;
                    }

                    break;

                case CAP_LONG_FLAG:
                    if(arg.value.longFlag.terminated) {
                        if(strcmp(arg.value.longFlag.str, "flag") == 0) {
                            results.hasFlag = 1;
                            EXPECT(arg.value.longFlag.attached) TO_BE_NULL;
                            EXPECT(arg.value.longFlag.length) TO_BE(4);                            
                        }
                    } else {
                        if(strncmp(arg.value.longFlag.str, "str", 3) == 0) {
                            results.hasStr = 1;
                            EXPECT(arg.value.longFlag.attached) TO_BE_STRING("val");
                            EXPECT(arg.value.longFlag.length) TO_BE(3);  
                        }
                    }

                    break;
            }
        }

        int expected[sizeof(results)/sizeof(int)];
        for(size_t i = 0; i < sizeof(expected)/sizeof(expected[0]); i++) {
            expected[i] = 1;
        }

        EXPECT(&results) TO_HAVE_BYTES(expected, sizeof(expected));
    }
}