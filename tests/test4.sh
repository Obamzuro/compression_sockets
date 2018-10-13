#!/bin/zsh
echo "#4 - Check stats"
# header
echo -n "\x53\x54\x52\x59\0\0\0\0" > check4
# info
echo -n "\0\0\0\x24\0\0\0\x35K" >> check4
../client localhost 2 > result4
if cmp check4 result4
then echo "\u001b[32m 4 - PASS \u001b[0m"
else echo "\u001b[31m 4 - WRONG! \u001b[0m"
fi
