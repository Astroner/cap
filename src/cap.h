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
