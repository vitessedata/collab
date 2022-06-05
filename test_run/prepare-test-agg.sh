#!/bin/bash

# Assume xrgdiv in PATH
xrgdiv -i csv -s test-agg.json test-agg.csv
rm test-agg.list test-agg.schema test-agg.zmp
mv test-agg_0.xrg in/test-agg.xrg
