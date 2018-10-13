#!/bin/zsh
echo "#9 - Zero test (check stats)"
# header
echo -n "\x53\x54\x52\x59\0\0\0\0" > check9
# info
echo -n "\0\0\0\x22\0\0\0\x21\0" >> check9
../client localhost 2 > result9
if cmp check9 result9
then echo "\u001b[32m 9 - PASS \u001b[0m"
else echo "\u001b[31m 9 - WRONG! \u001b[0m"
fi
