#!/bin/bash
rm -f ./xrun
ln -s ../xrun
mkdir -p out

for index in {1..1065} q{1..6}; do
    rm -f out/$index
    if ! [ -f in/$index.plan ]; then
	continue
    fi
    echo test $index
    ./xrun in/$index.plan >& out/$index.out
    diff good/$index.out out/$index.out || { echo '--- FAILED ---'; exit 1; }
done
echo DONE
