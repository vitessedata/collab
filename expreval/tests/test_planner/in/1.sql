-- simple query
select l_returnflag, l_linestatus, l_orderkey  
from lineitem
where 30 <= l_orderkey and l_orderkey <= 40;
