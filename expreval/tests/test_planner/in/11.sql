select l_linestatus
from lineitem where l_linestatus not in ('O', 'A');
