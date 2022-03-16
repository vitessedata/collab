## Make the xrg files
1. download https://drive.google.com/file/d/1Oik5nD_9QCVijb-51zfa56ZaDQBPvJo8/view
2. save as lineitem.csv.gz
3. mkdir -p ./drive{1,2,3}/lineitem
4. `./xrgdiv -i csv -D ./drive1/lineitem -D ./drive2/lineitem -D ./drive3/lineitem -s lineitem.schema -d '|' lineitem.csv.gz`

## Run the sql

./xplanner -D ./drive1/lineitem -D ./drive2/lineitem -D ./drive3/lineitem  lineitem.schema simple.sql

