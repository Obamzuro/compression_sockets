#!/bin/zsh
#../server 4000
../client localhost 3 >&-
for ((i=0; i<=9; ++i))
do
	zsh test$i.sh;
done
