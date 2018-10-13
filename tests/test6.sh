#!/bin/zsh
echo "#6 - Check compress test with payload with not lowercase letters"
# header
echo -n "\x53\x54\x52\x59\0\0\0\x25" > check6
# info
../client localhost 4 asdfqwe123 > result6
if cmp check6 result6
then echo "\u001b[32m 6 - PASS \u001b[0m"
else echo "\u001b[31m 6 - WRONG! \u001b[0m"
fi
