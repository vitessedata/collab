## Make the xrg files
1. download https://drive.google.com/file/d/1Oik5nD_9QCVijb-51zfa56ZaDQBPvJo8/view
    (alternately https://s3.amazonaws.com/vitessedata/download/lineitem.csv.gz )
2. save as lineitem.csv.gz
3. mkdir -p ./disk{1,2,3}/lineitem
4. `./xrgdiv -i csv -D ./disk1/lineitem -D ./disk2/lineitem -D ./disk3/lineitem -s lineitem.schema -d '|' lineitem.csv.gz`

## Run the sql

LD_LIBRARY_PATH=.
./xplanner -d <absolute_path>/disk1 -d <absolute_path>/disk2 -d <absolute_path>/disk3  lineitem.schema simple.sql



