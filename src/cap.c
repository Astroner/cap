#include "cap.h"

#include <stddef.h>

void Cap_Init(int argc, char** argv, struct Cap_Iterator* iterator) {
    iterator->argc = argc;
    iterator->argv = argv;
    iterator->index = 0;
    iterator->mergedFlagsCursor = NULL;
}

int CapInternalRead(Cap_Iterator* iterator, Cap_Item* item, int isDry) {
    if(iterator->mergedFlagsCursor) {
        char nextChar = iterator->mergedFlagsCursor[0];
        if(item) {
            item->type = CAP_FLAG;
            item->value.flag.ch = nextChar;
            item->value.flag.attached = NULL;
        }

        switch(iterator->mergedFlagsCursor[1]) {
            case '\0':
                if(!isDry) iterator->mergedFlagsCursor = NULL;
                break;
            
            case '=':
                if(iterator->mergedFlagsCursor[2]) {
                    item->value.flag.attached = iterator->mergedFlagsCursor + 2;
                }
                if(!isDry) iterator->mergedFlagsCursor = NULL;
                break;
            
            default:
                if(!isDry) iterator->mergedFlagsCursor = iterator->mergedFlagsCursor + 1;
        }

        return 1;
    }

    if(iterator->index >= iterator->argc) {
        return 0;
    }

    char* arg = iterator->argv[iterator->index];

    if(arg[0] == '-') {
        char* cursor = arg + 1;
        char ch;
        if(arg[1] == '-') {
            cursor = cursor + 1;

            if(item) {
                item->type = CAP_LONG_FLAG;
                item->value.longFlag.str = arg + 2;
                item->value.longFlag.length = 0;
                item->value.longFlag.terminated = 1;
                item->value.longFlag.attached = NULL;
            }

            while((ch = *cursor++)) {
                if(ch == '=') {
                    if(item) {
                        item->value.longFlag.terminated = 0;
                        if(cursor[0]) {
                            item->value.longFlag.attached = cursor;
                        }
                    }

                    break;
                } else if(item) {
                    item->value.longFlag.length += 1;
                }
            }
        } else {
            char ch = cursor[0];

            if(item) {
                item->type = CAP_FLAG;
                item->value.flag.ch = ch;
                item->value.flag.attached = NULL;
            }

            switch(cursor[1]) {
                case '\0':
                    break;
                
                case '=':
                    if(cursor[2] && item) {
                        item->value.flag.attached = cursor + 2;
                    }

                    break;
                
                default: 
                    if(!isDry) iterator->mergedFlagsCursor = cursor + 1;
            }
        }
    } else if(item) {
        item->type = CAP_ARG;
        item->value.arg = arg;
    }

    if(!isDry) iterator->index++;

    return 1;
}

int Cap_Next(Cap_Iterator* iterator, Cap_Item* item) {
    return CapInternalRead(iterator, item, 0);
}

int Cap_Check(Cap_Iterator* iterator, Cap_Item* item) {
    return CapInternalRead(iterator, item, 1);
}
