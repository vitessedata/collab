SELECT l_returnflag,l_linestatus,SUM(l_quantity),SUM(l_extendedprice),SUM(l_extendedprice * (1.000000 - l_discount)),SUM((l_extendedprice * (1.000000 - l_discount)) * (1.000000 + l_tax)),SUM(l_quantity),COUNT(*),SUM(l_extendedprice),COUNT(*),SUM(l_discount),COUNT(*),COUNT(*) FROM "lineitem*" WHERE l_shipdate <= '1998-09-02' GROUP BY l_returnflag,l_linestatus
