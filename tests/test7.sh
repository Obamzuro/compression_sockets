#!/bin/zsh
echo "#7 - Check stats"
# header
echo -n "\x53\x54\x52\x59\0\0\0\0" > check7
# info
echo -n "\0\0\0\x12\0\0\0\x8\0" >> check7
../client/client localhost 2 > result7
if cmp check7 result7
then echo "\u001b[32m 7 - PASS \u001b[0m"
else echo "\u001b[31m 7 - WRONG! \u001b[0m"
fi
