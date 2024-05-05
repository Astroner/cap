# Hi there!
This is Cap, a single-file CLI arguments parsing lib for C.

## Supported formats
 - common arguments - ```program arg1 arg2 arg3```
 - single char flags - ```program -b -d```
 - concatenated single char flags - ```program -abcd```
 - long flags - ```program --flag```
 - attached to flag values - ```program -a=1 -bc=23 --flag=45```