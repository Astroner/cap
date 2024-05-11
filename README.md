# Hi there!
This is Cap, a [single-file](https://raw.githubusercontent.com/Astroner/cap/master/cap.h) CLI arguments parsing lib for C.

## Table of content
 - [Supported formats](#supported-formats)
 - [How to use](#how-to-use)
 - [Helper functions](#helper-functions)
     - [Cap_Check](#cap_check)
     - [Cap_Value](#cap_value)
 - [Helper macros](#helper-macros)
     - [CAP_FOR_EACH](#cap_for_each)
     - [CAP_PARSE_SWITCH](#cap_parse_switch)
         - [CAP_FLAGS](#cap_flags)
             - [CAP_MATCH_FLAG](#cap_match_flag)
             - [CAP_UNMATCHED_FLAGS](#cap_unmatched_flags)
         - [CAP_LONG_FLAGS](#cap_long_flags)
             - [CAP_MATCH_LFLAG](#cap_match_lflag)
             - [CAP_UNMATCHED_LFLAGS](#cap_unmatched_lflags)
         - [CAP_ARGS](#cap_args)


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

## Helper macros
### CAP_FOR_EACH
This macro simplifies iteration over the arguments:
```c
#include <stdio.h>

#define CAP_IMPLEMENTATION
#include "cap.h"

int main(int argc, char** argv) {
    if(argc < 2) printf("No arguments provided");
    else CAP_FOR_EACH(argc - 1, argv + 1, args, arg) {
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
In this example we do basically the same what we do in the [first example](#how-to-use), but with macro.
> **CAP_FOR_EACH** works as a single statement, so it can be used right after **else** and e.t.c.

```c
#define CAP_FOR_EACH(ARGC, ARGV, ARGS_NAME, ARG_NAME)
```
 - **ARGC** - **int** - number of arguments
 - **ARGV** - __char**__ - arguments
 - **ARGS_NAME** - Cap_Iterator variable name
 - **ARG_NAME** - Cap_Item variable name

### CAP_PARSE_SWITCH
This macro generalize arguments parsing with **switch** statement
```c
#include <stdio.h>

#define CAP_IMPLEMENTATION
#include "cap.h"

int main(int argc, char** argv) {
    if(argc < 2) printf("No args\n");
    else CAP_PARSE_SWITCH(argc - 1, argv + 1) {
        CAP_FLAGS(
            CAP_MATCH_FLAG('o', {
                char* value = Cap_getFlagValue();
                printf("-o %s\n", value);
            })
            CAP_UNMATCHED_FLAGS(ch, {
                printf("Unknown flag -%c\n", ch);

                return 1;
            })
        )
        CAP_LONG_FLAGS(
            CAP_MATCH_LFLAG("input", {
                char* value = Cap_getFlagValue();

                printf("--input %s\n", value);
            })
            CAP_UNMATCHED_LFLAGS(name, {
                printf("Unknown flag --%s\n", name->str);

                return 1;
            })
        )
        CAP_ARGS(value, {
            printf("Argument: %s\n", value);
        })
    }

    return 0;
}
```
> **CAP_PARSE_SWITCH** works as a single statement, so it can be used right after **else** and e.t.c.

```c
#define CAP_PARSE_SWITCH(ARGC, ARGV)
```
 - **ARGC** - **int** - number of arguments
 - **ARGV** - __char**__ - arguments

After initial macro statement goes code block with **CAP_FLAGS**, **CAP_LONG_FLAGS** and **CAP_ARGS** to parse single char flags, long flags and general args accordingly. **It is not required to provide all of these blocks.**

Macro function **Cap_getFlagValue()** returns current flag value.
```c
char* Cap_getFlagValue();
```

#### CAP_FLAGS
This macro initialize single char flags parsing block. It can consist of multiple **CAP_MATCH_FLAG** macro and single **CAP_UNMATCHED_FLAGS** macro at the end.
```c
#include <stdio.h>

#define CAP_IMPLEMENTATION
#include "cap.h"

int main(int argc, char** argv) {
    CAP_PARSE_SWITCH(argc - 1, argv + 1) {
        CAP_FLAGS(
            CAP_MATCH_FLAG('o', {
                char* value = Cap_getFlagValue();
                printf("-o %s\n", value);
            })
            CAP_UNMATCHED_FLAGS(ch, {
                printf("Unknown flag -%c\n", ch);

                return 1;
            })
        )
    }

    return 0;
}
```

##### CAP_MATCH_FLAG
Matches single char flag:
```c
#include <stdio.h>

#define CAP_IMPLEMENTATION
#include "cap.h"

int main(int argc, char** argv) {
    CAP_PARSE_SWITCH(argc - 1, argv + 1) {
        CAP_FLAGS(
            CAP_MATCH_FLAG('o', {
                char* value = Cap_getFlagValue();
                printf("-o %s\n", value);
            })
        )
    }

    return 0;
}
```
This code snippet matches flag **-o** and it's value.

```c
#define CAP_MATCH_FLAG(CHAR, CODE)
```
 - **CHAR** - **char** - flag character
 - **CODE** - code block to perform with the flag

##### CAP_UNMATCHED_FLAGS
Handles unmatched flags

```c
#include <stdio.h>

#define CAP_IMPLEMENTATION
#include "cap.h"

int main(int argc, char** argv) {
    CAP_PARSE_SWITCH(argc - 1, argv + 1) {
        CAP_FLAGS(
            CAP_UNMATCHED_FLAGS(ch, {
                printf("Unknown flag -%c\n", ch);
            })
        )
    }

    return 0;
}
```
This code snippet matches all the unhandled flags. 

```c
#define CAP_UNMATCHED_FLAGS(CHAR, CODE)
```
 - **CHAR** - **char** variable name to store unmatched flag character
 - **CODE** - code block to perform

#### CAP_LONG_FLAGS
This macro initialize multi char flags parsing block. It can consist of multiple **CAP_MATCH_LFLAG** macro and single **CAP_UNMATCHED_LFLAGS** macro at the end.
```c
#include <stdio.h>

#define CAP_IMPLEMENTATION
#include "cap.h"

int main(int argc, char** argv) {
    CAP_PARSE_SWITCH(argc - 1, argv + 1) {
        CAP_LONG_FLAGS(
            CAP_MATCH_LFLAG("input", {
                char* value = Cap_getFlagValue();

                printf("--input %s\n", value);
            })
            CAP_UNMATCHED_LFLAGS(name, {
                printf("Unknown flag --%s\n", name->str);

                return 1;
            })
        )
    }

    return 0;
}
```

##### CAP_MATCH_LFLAG
Matches multi-char flag:
```c
#include <stdio.h>

#define CAP_IMPLEMENTATION
#include "cap.h"

int main(int argc, char** argv) {
    CAP_PARSE_SWITCH(argc - 1, argv + 1) {
        CAP_LONG_FLAGS(
            CAP_MATCH_LFLAG("input", {
                char* value = Cap_getFlagValue();

                printf("--input %s\n", value);
            })
        )
    }

    return 0;
}
```
This code snippet matches flag **--input** and prints it's value

```c
#define CAP_MATCH_LFLAG(NAME, CODE)
```
 - **NAME** - __char*__ - flag name
 - **CODE** - code block to perform

##### CAP_UNMATCHED_LFLAGS
Handles unpatched multi-char flags
```c
#include <stdio.h>

#define CAP_IMPLEMENTATION
#include "cap.h"

int main(int argc, char** argv) {
    CAP_PARSE_SWITCH(argc - 1, argv + 1) {
        CAP_LONG_FLAGS(
            CAP_UNMATCHED_LFLAGS(name, {
                printf("Unknown flag --%s\n", name->str);

                return 1;
            })
        )
    }

    return 0;
}
```
This code snippet matches and prints all the unknown long flags.

```c
#define CAP_UNMATCHED_LFLAGS(NAME, CODE)
```
 - **NAME** - **Cap_LongFlag** variable name to store 
 - **CODE** - code block to perform

```c
typedef struct Cap_LongFlag {
    char* attached;
    char* str;
    int length;
    int terminated;
} Cap_LongFlag;
```

#### CAP_ARGS
This macro parses all the general args

```c
#include <stdio.h>

#define CAP_IMPLEMENTATION
#include "cap.h"

int main(int argc, char** argv) {
    CAP_PARSE_SWITCH(argc - 1, argv + 1) {
        CAP_ARGS(value, {
            printf("Argument: %s\n", value);
        })
    }

    return 0;
}
```
This code snippet prints out all the general arguments
```c
#define CAP_ARGS(NAME, CODE)
```
 - **NAME** - __char*__ variable name to store the argument
 - **CODE** - code block to perform