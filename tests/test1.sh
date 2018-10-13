#!/bin/zsh
echo "#1 - Check stats"
# header
echo -n "\x53\x54\x52\x59\0\0\0\0" > check1
# info
echo -n "\0\0\0\x8\0\0\0\x8\0" >> check1
../client/client localhost 2 > result1
if cmp check1 result1
then echo "\u001b[32m 1 - PASS \u001b[0m"
else echo "\u001b[31m 1 - WRONG! \u001b[0m"
fi
