-- test groupby without any agg functions
select l_returnflag
from lineitem
group by l_returnflag, l_linestatus
