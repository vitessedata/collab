select ((l_tax + 1) * 20), l_tax, count(*)
from lineitem
group by l_tax;
