#!/bin/zsh
echo "#2 - Check stats"
# header
echo -n "\x53\x54\x52\x59\0\0\0\0" > check2
# info
echo -n "\0\0\0\x10\0\0\0\x19\x0" >> check2
../client localhost 2 > result2
if cmp check2 result2
then echo "\u001b[32m 2 - PASS \u001b[0m"
else echo "\u001b[31m 2 - WRONG! \u001b[0m"
fi
