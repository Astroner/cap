#include "cap.h"

#include <stddef.h>
#include <stdio.h>

void Cap_Init(int argc, char** argv, struct Cap_Iterator* iterator) {
    iterator->argc = argc;
    iterator->argv = argv;
    iterator->index = 0;
    iterator->mergedFlagsCursor = NULL;
    iterator->argsOnly = 0;
}

int CapInternalParse(char* arg, Cap_Item* result, Cap_Iterator* iterator, int argsOnly) {
    if(arg[0] == '-' && !argsOnly) {
        char* cursor = arg + 1;
        char ch;
        if(arg[1] == '-') {
            cursor = cursor + 1;

            if(arg[2] == '\0') {
                return 0;
            }

            if(result) {
                result->type = CAP_LONG_FLAG;
                result->value.longFlag.str = arg + 2;
                result->value.longFlag.length = 0;
                result->value.longFlag.terminated = 1;
                result->value.longFlag.attached = NULL;
            }

            while((ch = *cursor++)) {
                if(ch == '=') {
                    if(result) {
                        result->value.longFlag.terminated = 0;
                        result->value.longFlag.attached = cursor;
                    }

                    break;
                } else if(result) {
                    result->value.longFlag.length += 1;
                }
            }
        } else if(arg[1] == '\0') {
            if(result) {
                result->type = CAP_ARG;
                result->value.arg = arg;
            }
        } else {
            char ch = cursor[0];

            if(result) {
                result->type = CAP_FLAG;
                result->value.flag.ch = ch;
                result->value.flag.attached = NULL;
            }

            switch(cursor[1]) {
                case '\0':
                    break;
                
                case '=':
                    if(result) {
                        result->value.flag.attached = cursor + 2;
                    }

                    break;
                
                default: 
                    if(iterator) iterator->mergedFlagsCursor = cursor + 1;
            }
        }
    } else if(result) {
        result->type = CAP_ARG;
        result->value.arg = arg;
    }

    if(iterator) iterator->index++;

    return 1;
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
                if(item) {
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
        if(item) {
            item->type = CAP_NONE;
        }

        return 0;
    }

    if(!CapInternalParse(iterator->argv[iterator->index], item, isDry ? NULL : iterator, iterator->argsOnly)) {
        if(isDry) {
            if(iterator->index + 1 < iterator->argc) {
                CapInternalParse(iterator->argv[iterator->index + 1], item, NULL, 1);

                return 1;
            }

            if(item) {
                item->type = CAP_NONE;
            }
            return 0;
        } else {
            iterator->argsOnly = 1;
            iterator->index = iterator->index + 1;
            return CapInternalRead(iterator, item, isDry);
        }
    }

    return 1;
}

int Cap_Next(Cap_Iterator* iterator, Cap_Item* item) {
    return CapInternalRead(iterator, item, 0);
}

int Cap_Check(Cap_Iterator* iterator, Cap_Item* item) {
    return CapInternalRead(iterator, item, 1);
}

char* Cap_Value(Cap_Iterator* iterator, Cap_Item* item) {
    if(item->type == CAP_ARG) return NULL;

    if(item->value.attached) return item->value.attached;

    Cap_Item value;
    Cap_Check(iterator, &value);

    if(value.type != CAP_ARG) return NULL;

    Cap_Next(iterator, NULL);

    return value.value.arg;
}

void Cap_Parse(char* arg, Cap_Item* result) {
    if(result) {
        result->type = CAP_NONE;
    }

    CapInternalParse(arg, result, NULL, 0);
}