-- negative test
-- expect error:
-- ERROR: column "l_discount" must appear in GROUP BY clause or be used in an aggregate function
select l_returnflag, 
	l_linestatus,
	sum(l_quantity) as sum_qty, 
	sum(l_extendedprice) as sum_base_price, 
	sum(l_extendedprice * (1 - l_discount)) as sum_disc_price, 
	sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)) as sum_charge, 
	sum(l_quantity) / count(l_quantity) as avg_qty, 
	sum(l_extendedprice) / count(l_extendedprice) /*avg(l_extendedprice)*/ as avg_price, 
	sum(l_discount)/count(l_discount) /*(avg(l_discount)*/ as avg_disc, 
	count(*) as count_order ,
	l_discount

from lineitem 
where l_shipdate <= date '1998-12-01' - interval '112 day' 
group by l_returnflag, l_linestatus;
