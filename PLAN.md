# PLAN

## PLAN syntax

    (PLAN {plan-tree} )

For example:

    (PLAN (FILTER {expr} (SCAN ...)))

## SCAN syntax

    (SCAN (SYMTAB ...) [(SYMBOL ...)] (PATH ...))

### SYMTAB syntax

    (SYMTAB (var1 type1) (var2 type2) (var3 type3) ...)

### SYMBOL syntax

    (SYMBOL sym1 sym2 sym3 ...)

## FILTER syntax

    (FILTER {expr} {kid})

## GROUPBY syntax

    (GROUPBY
        [(BUCKET (var c1) (var c2))]
        (FUNCTION (COUNT (int 1)) (SUM (* (var c3) (var c4))))
	{kid})

## PROJECT syntax

    (PROJECT (COLUMN (name expr1) (name expr2) ...) {kid})


