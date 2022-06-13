#!/bin/bash
F=xrun
rm -f $F
[ -f ../src/$F ] && ln -s ../src/$F || ln -s ../$F

mkdir -p out
for index in {1..1067} q{1..6}; do
    [ -f in/$index.plan ] || continue
    echo test $index
    ./$F in/$index.plan >& out/$index.out
    diff good/$index.out out/$index.out || { echo '--- FAILED ---'; exit 1; }
done
echo DONE
