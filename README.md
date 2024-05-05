# Hi there!
This is Cap, a [single-file](https://raw.githubusercontent.com/Astroner/cap/master/cap.h) CLI arguments parsing lib for C.

## Table of content
 - [Supported formats](#supported-formats)
 - [How to use](#how-to-use)
 - [Helper functions](#helper-functions)
     - [Cap_Check](#cap_check)
     - [Cap_Value](#cap_value)

## Supported formats
 - common arguments - ```program arg1 arg2 arg3```
 - single char flags - ```program -b -d```
 - concatenated single char flags - ```program -abcd```
 - long flags - ```program --flag```
 - attached to flag values - ```program -a=1 -bc=23 --flag=45```

## How to use
First of all, include library file [cap.h](https://raw.githubusercontent.com/Astroner/cap/master/cap.h) and include the implementation by defining **CAP_IMPLEMENTATION** before *.h* file once somewhere in the project.

```c
#define CAP_IMPLEMENTATION
#include "cap.h"
```

Cap does not parses all the arguments at once, instead it provides a convenient way to iterate over them.
```c
#include <stdio.h>

#define CAP_IMPLEMENTATION
#include "cap.h"

int main(int argc, char** argv) {
    Cap_Iterator args;
    Cap_Init(argc, argv, &args);

    Cap_Item arg;
    while(Cap_Next(&args, &arg)) {
        switch(arg.type) {
            case CAP_FLAG:
                printf("Single char flag: %c\n", arg.value.flag.ch);
                break;

            case CAP_LONG_FLAG:
                printf("Long flag: %s\n", arg.value.longFlag.str);
                break;

            case CAP_ARG:
                printf("General arg: %s\n", arg.value.arg);
                break;
        }
    }

    return 0;
}
```
First, we define **iterator**, then we initialize it by calling **Cap_Init()** with **argc**, **argv**, and then we use **Cap_Next()** function to actually iterate over the arguments. **Cap_Item** stores information about the current argument:
```c
typedef struct Cap_Item {
    Cap_ItemType type;
    Cap_ItemValue value;
} Cap_Item;
```

**Cap_ItemType** is an enum that stores info about argument type:
```c
typedef enum Cap_ItemType {
    CAP_FLAG = 0, // single char flag
    CAP_LONG_FLAG = 1, // long flag
    CAP_ARG = 2, // general arg
} Cap_ItemType;
```

**Cap_ItemValue** is a union that conditionally stores info related to the type:
```c
typedef union Cap_ItemValue {
    char* attached; // value attached by '=' (only for flags)

    struct {
        char* attached; // value attached by '='
        char ch;
    } flag; // single char flag

    char* arg; // general arg

    struct {
        char* attached; // value attached by '='
        char* str;
        int length;
        int terminated; // is the flag string nul-terminated
    } longFlag; // long flag
} Cap_ItemValue;
```

## Helper functions
### Cap_Check
This function checks next argument without moving the iterator:
```c
int Cap_Check(Cap_Iterator* iterator, Cap_Item* item);
```
 - **returns** - 0 if no args left else returns 1
 - **iterator** - arguments iterator
 - **item** - **Cap_Item** to store argument

### Cap_Value
Returns flag value(if some) and moves the iterator forward:
```c
char* Cap_Value(Cap_Iterator* iterator, Cap_Item* item);
```
 - **returns** - **NULL** if the flag has no value, else returns pointer to it
 - **iterator** - arguments iterator
 - **item** - flag to check

```c
#include <stdio.h>

#define CAP_IMPLEMENTATION
#include "cap.h"

int main(void) {
    char* argv[] = { "-d=2" };
    int argc = 1;

    Cap_Iterator args;
    Cap_Init(argc, argv, &args);

    Cap_Item d;
    Cap_Next(&args, &d);
    char* value = Cap_Value(&args, &d);
    printf("%s\n", value); // 2

    return 0;
}
```