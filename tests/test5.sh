#!/bin/zsh
echo "#5 - Reset stats"
# header
echo -n "\x53\x54\x52\x59\0\0\0\0" > check5
../client localhost 3 > result5
if cmp check5 result5
then echo "\u001b[32m 5 - PASS \u001b[0m"
else echo "\u001b[31m 5 - WRONG! \u001b[0m"
fi
