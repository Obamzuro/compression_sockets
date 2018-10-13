#!/bin/zsh
echo "#0 - Test ping"
# header
echo -n "\x53\x54\x52\x59\0\0\0\0" > check0
../client localhost 1 > result0
if cmp check0 result0
then echo "\u001b[32m 0 - PASS \u001b[0m"
else echo "\u001b[31m 0 - WRONG! \u001b[0m"
fi
