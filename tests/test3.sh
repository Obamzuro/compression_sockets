#!/bin/zsh
echo "#3 - Compress test"
# header
echo -n "\x53\x54\x52\x59\0\x3\0\0" > check3
# info
echo -n "\x33ab" >> check3
../client localhost 4 aaab > result3
if cmp check3 result3
then echo "\u001b[32m 3 - PASS \u001b[0m"
else echo "\u001b[31m 3 - WRONG! \u001b[0m"
fi
