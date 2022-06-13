-- simple OR query; this should produce 0 results.
select l_returnflag, l_linestatus, l_orderkey  
from lineitem
where l_orderkey < 0 or l_orderkey > 1000;
