#!/bin/bash
rm -f xplan 
ln -s ../xplan
mkdir -p out

for fname in {1..1} n{1..1} q{1..6}; do
    rm -f out/$fname.*
    if ! [ -f in/$fname.sql ]; then
	continue
    fi
    echo test $fname
    ./xplan in/$fname.sql &> out/$fname.out
    diff good/$fname.out out/$fname.out || { echo '--- FAILED ---'; exit 1; }
done

echo DONE
