#!/bin/zsh
echo "#10 - Compress payload with wrong length"
# header
echo -n "\x53\x54\x52\x59\0\0\0\x10" > check10
../client localhost 4 asdfqwer > result10
if cmp check10 result10
then echo "\u001b[32m 10 - PASS \u001b[0m"
else echo "\u001b[31m 10 - WRONG! \u001b[0m"
fi
