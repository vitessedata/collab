#!/bin/bash
F=xplan
rm -f $F
[ -f ../src/$F ] && ln -s ../src/$F || ln -s ../$F

mkdir -p out
for fname in {1..1067} n{1..2} q{1..6}; do
    [ -f in/$fname.sql ] || continue
    echo test $fname
    ./$F in/$fname.sql &> out/$fname.out
    diff good/$fname.out out/$fname.out || { echo '--- FAILED ---'; exit 1; }
done
echo DONE
