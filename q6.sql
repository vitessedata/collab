SELECT SUM(l_extendedprice * l_discount) FROM "lineitem*" WHERE l_shipdate >= '1994-01-01' AND l_shipdate < '1995-01-01' AND l_discount >= 0.050000 AND l_discount <= 0.070000 AND l_quantity < 24.000000