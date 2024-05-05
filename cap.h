#if !defined(CAP_H)
#define CAP_H

typedef enum Cap_ItemType {
    CAP_FLAG = 0,
    CAP_LONG_FLAG = 1,
    CAP_ARG = 2,
} Cap_ItemType;

typedef union Cap_ItemValue {
    char* attached;
    struct {
        char* attached;
        char ch;
    } flag;

    char* arg;

    struct {
        char* attached;
        char* str;
        int length;
        int terminated;
    } longFlag;
} Cap_ItemValue;

typedef struct Cap_Item {
    Cap_ItemType type;
    Cap_ItemValue value;
} Cap_Item;

typedef struct Cap_Iterator {
    int argc;
    char** argv;
    int index;
    char* mergedFlagsCursor;
    Cap_Item current;
} Cap_Iterator;

void Cap_Init(int argc, char** argv, struct Cap_Iterator* iterator);
int Cap_Next(Cap_Iterator* iterator, Cap_Item* item);
int Cap_Check(Cap_Iterator* iterator, Cap_Item* item);

char* Cap_Value(Cap_Iterator* iterator, Cap_Item* item);

#endif // CAP_H

#if defined(CAP_IMPLEMENTATION)

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

char* Cap_Value(Cap_Iterator* iterator, Cap_Item* item) {
    if(item->type == CAP_ARG) return NULL;

    if(item->value.attached) return item->value.attached;

    Cap_Item value;
    Cap_Check(iterator, &value);

    if(value.type != CAP_ARG) return NULL;

    Cap_Next(iterator, NULL);

    return value.value.arg;
}
#endif // CAP_IMPLEMENTATION
