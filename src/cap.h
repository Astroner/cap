// Docs: https://github.com/Astroner/cap

#if !defined(CAP_H)
#define CAP_H

#if !defined(CAP_STRN_CMP)
    #include <string.h>
    #define CAP_STRN_CMP strncmp
#endif // CAP_STR_CMP

#define CAP_NONE -1
#define CAP_FLAG 0
#define CAP_LONG_FLAG 1
#define CAP_ARG 2

typedef struct Cap_LongFlag {
    char* attached;
    char* str;
    int length;
    int terminated;
} Cap_LongFlag;

// Types
typedef union Cap_ItemValue {
    char* attached;
    struct {
        char* attached;
        char ch;
    } flag;

    char* arg;

    Cap_LongFlag longFlag;
} Cap_ItemValue;

typedef struct Cap_Item {
    int type;
    Cap_ItemValue value;
} Cap_Item;

typedef struct Cap_Iterator {
    int argc;
    char** argv;
    int index;
    char* mergedFlagsCursor;
} Cap_Iterator;

// Functions
void Cap_Init(int argc, char** argv, struct Cap_Iterator* iterator);
int Cap_Next(Cap_Iterator* iterator, Cap_Item* item);
int Cap_Check(Cap_Iterator* iterator, Cap_Item* item);

char* Cap_Value(Cap_Iterator* iterator, Cap_Item* item);
void Cap_Parse(char* arg, Cap_Item* result);

// Macros
/**
 * ARGC - int - number of arguments
 * ARGV - char** - arguments
 * ARGS_NAME - Cap_Iterator name
 * ARG_NAME - Cap_Item name
 * 
 * Iterate over the CLI arguments
 * 
 * Example:
 * int main(int argc, char** argv) {
 *      CAP_FOR_EACH(argc, argv, iterator, arg) {
 *          // ... do smth with args
 *      }
 * }
*/
#define CAP_FOR_EACH(ARGC, ARGV, ARGS_NAME, ARG_NAME)\
    for(Cap_Iterator ARGS_NAME, *CAP_ARGS_NAME_LIFETIME = (Cap_Init(ARGC, ARGV, &ARGS_NAME), (void*)0x0); CAP_ARGS_NAME_LIFETIME != (void*)0x1;)\
        for(Cap_Item ARG_NAME; CAP_ARGS_NAME_LIFETIME != (void*)0x1; CAP_ARGS_NAME_LIFETIME = (void*)0x1)\
            while(Cap_Next(&ARGS_NAME, &ARG_NAME))

/**
 * ARGC - int - number of arguments
 * ARGV - char** - arguments
 * 
 * Macro to simplify general arguments parsing
*/
#define CAP_PARSE_SWITCH(ARGC, ARGV)\
    CAP_FOR_EACH(ARGC, ARGV, CAP_LOCAL_ARGS, CAP_LOCAL_ARG)\
        switch(CAP_LOCAL_ARG.type)

/**
 * Only to use inside of CAP_PARSE_SWITCH
 * 
 * Parse single char flags
 * 
 * Example:
 * CAP_FLAGS(
 *      CAP_MATCH_FLAG('a', {
 *          // ...
 *      })
 *      // ...
 * )
*/
#define CAP_FLAGS(...)\
    case CAP_FLAG:\
        switch(CAP_LOCAL_ARG.value.flag.ch) {\
            __VA_ARGS__\
        }\
        break;

/**
 * Only to use inside of CAP_FLAGS
 * 
 * Match single char flag(-d, -o and e.t.c)
 * 
 * Example:
 * CAP_FLAGS(
 *      CAP_MATCH_FLAG('a', {
 *          // code...
 *      })
 *      // ...
 * )
*/
#define CAP_MATCH_FLAG(CHAR, CODE)\
    case CHAR:\
        CODE\
        continue;

/**
 * Only to use inside of CAP_FLAGS and before CAP_MATCH_FLAG
 * 
 * Add additional flag to match with the next CAP_MATCH_FLAG statement
 * 
 * CAP_FLAGS(
 *      CAP_ADD_FLAG_MATCH('A')
 *      CAP_MATCH_FLAG('a', {
 *          if(Cap_getCurrentFlag() == 'A') {
 *              // do additional actions
 *          }
 *          // code...
 *      })
 *      // ...
 * )
*/
#define CAP_ADD_FLAG_MATCH(CHAR)\
    case CHAR:

/**
 * Only to use inside of CAP_FLAGS
 * 
 * Handle unmatched flag
 * 
 * CHAR - char variable name with unmatched flag
 * CODE - code to perform
 * 
 * Example:
 * CAP_FLAGS(
 *      CAP_UNMATCHED_FLAGS(ch, {
 *          printf("Unknown flag -%c\n", ch);
 *      })
 *      // ...
 * )
*/
#define CAP_UNMATCHED_FLAGS(CHAR, CODE)\
    default: {\
        char CHAR = CAP_LOCAL_ARG.value.flag.ch;\
        CODE\
        break;\
    }

/**
 * Only to use inside of CAP_PARSE_SWITCH
 * 
 * Parse plain arguments
 * 
 * NAME - name of the variable to store char* argument
 * CODE - code block
 * 
 * Example:
 * CAP_ARGS(arg, {
 *      // ... do smth with arg
 * })
*/
#define CAP_ARGS(NAME, CODE)\
    case CAP_ARG: {\
        char* NAME = CAP_LOCAL_ARG.value.arg;\
        CODE\
        continue;\
    }

/**
 * Only to use inside of CAP_PARSE_SWITCH
 * 
 * Parse string flags(--flag, --value and e.t.c.)
 * 
 * Example:
 * CAP_LONG_FLAGS(
 *      CAP_MATCH_LFLAG("value", {
 *          // ...
 *      })
 *      // ...
 * )
*/
#define CAP_LONG_FLAGS(...)\
    case CAP_LONG_FLAG:\
        __VA_ARGS__\
        break;


/**
 * Only to use inside of CAP_LONG_FLAGS
 * 
 * Match multi-char flag
 * 
 * NAME - char* - flag name
 * CODE - code block
 * 
 * Example:
 * CAP_LONG_FLAGS(
 *      CAP_MATCH_LFLAG("value", {
 *          // ...
 *      })
 *      // ...
 * )
*/
#define CAP_MATCH_LFLAG(NAME, CODE)\
    if(CAP_STRN_CMP(CAP_LOCAL_ARG.value.longFlag.str, NAME, CAP_LOCAL_ARG.value.longFlag.length) == 0) {\
        CODE\
        continue;\
    }

/**
 * Only to use inside of CAP_LONG_FLAGS
 * 
 * Handle unmatched long flag
 * Should go after all other matches
 * 
 * NAME - Cap_LongFlag* variable name
 * CODE - code block
 * 
 * Example:
 * CAP_LONG_FLAGS(
 *      CAP_UNMATCHED_LFLAGS(name, {
 *          printf("Unknown flag --%s\n", name->str);
 *      })
 * )
*/
#define CAP_UNMATCHED_LFLAGS(NAME, CODE)\
    {\
        Cap_LongFlag* NAME = &CAP_LOCAL_ARG.value.longFlag;\
        CODE\
    }

/**
 * Only to use inside of CAP_MATCH_FLAG or CAP_MATCH_LFLAG
 * 
 * Get value of the current flag if some
*/
#define Cap_getFlagValue() Cap_Value(&CAP_LOCAL_ARGS, &CAP_LOCAL_ARG)

/**
 * Only to use inside of CAP_MATCH_FLAG, CAP_MATCH_LFLAG or CAP_ARGS
 * 
 * Check next Cap_Item(equivalent of Cap_Check buf for CAP_PARSE_SWITCH)
 * 
 * NAME - Cap_Item variable name
 * 
 * Example:
 * CAP_FLAGS(
 *      CAP_MATCH_FLAG('o', {
 *          CAP_CHECK_NEXT(conditionalValue) {
 *              printf("%d\n", conditionalValue.type);
 *          } 
 *      })
 * )
*/
#define CAP_CHECK_NEXT(NAME)\
    for(Cap_Item NAME, *CAP_LOCAL_CHECK_LIFETIME = (Cap_Check(&CAP_LOCAL_ARGS, &NAME), (void*)0x0); CAP_LOCAL_CHECK_LIFETIME != (void*)0x1; CAP_LOCAL_CHECK_LIFETIME = (void*)0x1)

/**
 * Only to use inside of CAP_CHECK_NEXT
 * 
 * Confirms the check and moves the iterator forward
 *
 * Example:
 * CAP_FLAGS(
 *      CAP_MATCH_FLAG('o', {
 *          CAP_CHECK_NEXT(conditionalValue) {
 *              printf("%d\n", conditionalValue.type);
 *              CAP_CHECK_CONFIRM()
 *          } 
 *      })
 * )
*/
#define CAP_CHECK_CONFIRM()\
    Cap_Next(&CAP_LOCAL_ARGS, &CAP_LOCAL_ARG)

/**
 * Only to use inside of CAP_MATCH_FLAG
 * 
 * Get current flag char
*/
#define Cap_getCurrentFlag() CAP_LOCAL_ARG.value.flag.ch

#endif // CAP_H
