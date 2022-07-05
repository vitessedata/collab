# Expression Support in SmartSSD Hardware Design

This document uses the
[S-expr](expreval/README.md#s-expression)
notion to elaborate the expressions currently supported in SmartSSD acceleration library.

- [Expression Support in SmartSSD Hardware Design](#expression-support-in-smartssd-hardware-design)
  - [String Variable](#string-variable)
  - [Non-string Variable](#non-string-variable)
  - [Casting Variable](#casting-variable)
    - [Promote Casting](#promote-casting)
    - [Demote Transformation](#demote-transformation)
    - [Other Casting](#other-casting)

## String Variable

* Comparison includes `==` and `LIKE`.
* Up to 8 `==` comparisons, each with literal no longer than 64B.
  Multiple comparisons can only be combined with `OR`.
  Alternatively, the expression can be `IN` with up to 8 literals, each within 64B.
* Or 1 `LIKE` comparison, with pattern within 64B. The pattern can only have wildcard (`%`)
  at the beginning or end of the pattern. So it's one of `%abc%`, `%abc`, `abc%` and `abc`.
  Wildcard in the middle like `a%bc` is not supported.
* Optional `NOT` is supported as expression root.
* Casting string to other datatype is not supported yet.

Examples:
```
(== (var x) (str "James Bond"))

(IN (var x) (str "cat) (str "dog"))
# alternatively
(OR (== (var x) (str "cat"))
    (== (var x) (str "dog")))

(NOT (IN (var x) (str "cat") (str "dog")))
# or
(NOT (OR (== (var x) (str "cat"))
         (== (var x) (str "dog"))))

(LIKE (var x) (str "%air%"))

(NOT (LIKE (var x) (str "%air%")))
```

**Unsupported** examples:
```
(OR (LIKE (var x) (str "%bus"))
    (LIKE (var x) (str "%truck")))
# Not supported as more than one LIKE comparision exist

(AND (== (var x) (str "bus"))
     (== (var x) (str "car)))
# Cannot combine predicates with AND for string var

(OR (== (var x) (str "car))
    (LIKE (var x) (str "%bus")))
# Cannot mix == and LIKE.

(>= (date (var x)) (date 1998-12-01))
# cannot cast string to date
```

## Non-string Variable

* No arithmetic (`+, -, *, /`) is supported yet.
* Comparison (`>, >=, <, <=, ==, !=`) with same-typed literal is supported.
* Up to 4 comparison leaf clause in an expression, with arbitrary logic (`AND, OR, NOT`)
  combination of results of these leaf clauses.
* Alternatively, the expression can be `IN` with up to 4 literals.
* No expression can be evaluated with `i128`, `decimal64` and `decimal128`.
  
Examples:
```
# Assume var x is of type i32
(OR (AND (>= (var x) (i32 3)) (<= (var x) (i32 5)))
    (AND (> (var x) (i32 10)) (< (var x) (i32 20))))

# Assume var x is of type fp32
(AND (> (var x) (fp32 1.5))
     (!= (var x) (fp32 3))
     (!= (var x) (fp32 4)))

# Assume var x is of type fp64
# WHERE x IN (0.5, 0.7)
(IN (var x) (fp64 0.5) (fp64 0.7))
# or
(OR (== (var x) (fp64 0.5))
    (== (var x) (fp64 0.7)))
```

**Unsupported** Examples:
```
# Assume var x is of type i32
(> (* (var x) (i32 3)) (i32 100))
# multiply is not supported
```

## Casting Variable

It is assumed that any casing of literal in expression has been evaluated before submission.

### Promote Casting

Supported *promote* casting operations are:
* `fp32` to `fp64`

Other type promotions like `i8` to `i32` are not supported in hardware, but supported by library
with *demote transformation* described in the next section.

Example:
```
# Assume var x is of type fp32
(AND (> (fp64 (var x)) (fp64 3.14159))
     (< (fp64 (var x)) (fp64 4.5)))
```

*The var must be same type in all comparsion expressions*.

**Unsupported** example:
```
# Assume var x is of type fp32
(AND (> (fp64 (var x)) (fp64 3.14159))
     (< (var x) (fp32 4.5))
# Cannot use var x both as fp32 and fp64
```

### Demote Transformation

A list of *demote* transformation has been implemented to reduce hardware design size.
Currently the translation is internally called during task submission,
later this could be exposed and called once by planner before submitting tasks.

Demote transformations are
* float literal (`fp32, fp64`) to integer literal of same type as variable
* wider integer literal (`i64, i32, i16`) to narrower integer of same type as variable
* `datetime` to `date`

Comparison operator would be adjusted accordingly, for example from `>` to `>=`.

Examples:
```
# Assume var x is i32
(> (fp32 (var x)) (fp32 1.5))
# would be transformed to
(>= (var x) (i32 2))

# Assume var x is i32
(> (i64 (var x)) (i64 500))
# would be transformed to
(> (var x) (i32 500))
```

The transformation can be check with detailed log enabled.

### Other Casting

This subsection loosely corresponds to the `CAST(... AS ...)` expression in SQL.

The SmartSSD API does *not support* casting between integral (`i8, i16, i32, i64, i128`), decimal,
floaping point, timing (`date, datetime, timestamp, interval`) and string.
