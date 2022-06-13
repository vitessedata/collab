-- simple OR query. This should produce 
-- l_orderkey == 1 or 194.
select l_returnflag, l_linestatus, l_orderkey  
from lineitem
where l_orderkey <= 1 or l_orderkey >= 194;
