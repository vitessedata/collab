-- best case of tpch q1

select
  count(l_returnflag),
  count(l_linestatus),
  count(l_quantity),
  count(l_extendedprice),
  count(l_discount),
  count(l_tax)
from
  lineitem_800k
where 
  l_shipdate <= date '1998-12-01' - interval '112 day'
;
