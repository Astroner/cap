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

    IT("reads values correctly") {
        char* argv[] = { "-a=22", "-b", "33", "-c" };
        char argc = sizeof(argv)/sizeof(argv[0]);

        Cap_Iterator args;
        Cap_Init(argc, argv, &args);

        Cap_Item arg;

        Cap_Next(&args, &arg);
        EXPECT(arg.type) TO_BE(CAP_FLAG);
        EXPECT(arg.value.flag.ch) TO_BE('a');
        EXPECT(Cap_Value(&args, &arg)) TO_BE_STRING("22");

        Cap_Next(&args, &arg);
        EXPECT(arg.type) TO_BE(CAP_FLAG);
        EXPECT(arg.value.flag.ch) TO_BE('b');
        EXPECT(Cap_Value(&args, &arg)) TO_BE_STRING("33");

        Cap_Next(&args, &arg);
        EXPECT(arg.type) TO_BE(CAP_FLAG);
        EXPECT(arg.value.flag.ch) TO_BE('c');
        EXPECT(Cap_Value(&args, &arg)) TO_BE_NULL;
    }

    IT("parses value correctly") {
        Cap_Item item;
        Cap_Parse("ss", &item);
        EXPECT(item.type) TO_BE(CAP_ARG);
        EXPECT(item.value.arg) TO_BE_STRING("ss");


        Cap_Parse("-s", &item);
        EXPECT(item.type) TO_BE(CAP_FLAG);
        EXPECT(item.value.flag.ch) TO_BE('s');
        EXPECT(item.value.flag.attached) TO_BE_NULL;

        Cap_Parse("-s=arg", &item);
        EXPECT(item.type) TO_BE(CAP_FLAG);
        EXPECT(item.value.flag.ch) TO_BE('s');
        EXPECT(item.value.flag.attached) TO_BE_STRING("arg");

        Cap_Parse("--flag", &item);
        EXPECT(item.type) TO_BE(CAP_LONG_FLAG);
        EXPECT(item.value.longFlag.str) TO_BE_STRING("flag");
        EXPECT(item.value.longFlag.length) TO_BE(4);
        EXPECT(item.value.longFlag.terminated) TO_BE_TRUTHY;
        EXPECT(item.value.longFlag.attached) TO_BE_NULL;

        Cap_Parse("--flag=value", &item);
        EXPECT(item.type) TO_BE(CAP_LONG_FLAG);
        EXPECT(item.value.longFlag.str) TO_HAVE_RAW_BYTES('f', 'l', 'a', 'g');
        EXPECT(item.value.longFlag.length) TO_BE(4);
        EXPECT(item.value.longFlag.terminated) TO_BE_FALSY;
        EXPECT(item.value.longFlag.attached) TO_BE_STRING("value");
    }
}