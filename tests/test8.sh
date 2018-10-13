#!/bin/zsh
echo "#8 - Zero test"
# header
echo -n "\x53\x54\x52\x59\0\0\0\x26" > check8
../client/client localhost 4 "" > result8
if cmp check8 result8
then echo "\u001b[32m 8 - PASS \u001b[0m"
else echo "\u001b[31m 8 - WRONG! \u001b[0m"
fi
