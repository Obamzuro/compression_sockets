#!/bin/zsh
#make -C ../server
#../server/server 4000 &
make -C ../client
../client/client localhost 3 >&-
for ((i=0; i<=9; ++i))
do
	zsh test$i.sh;
done
kill %1
