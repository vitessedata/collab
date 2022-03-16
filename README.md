## Make the xrg files
1. download https://disk.google.com/file/d/1Oik5nD_9QCVijb-51zfa56ZaDQBPvJo8/view
2. save as lineitem.csv.gz
3. mkdir -p ./disk{1,2,3}/lineitem
4. `./xrgdiv -i csv -D ./disk1/lineitem -D ./disk2/lineitem -D ./disk3/lineitem -s lineitem.schema -d '|' lineitem.csv.gz`

## Run the sql

./xplanner -d ./disk1 -d ./disk2 -d ./disk3  lineitem.schema simple.sql



