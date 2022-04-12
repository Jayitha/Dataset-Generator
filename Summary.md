Paper: [[The Skyline Operator.pdf]]

## Independent

All attributes are generated independently using a uniform distribution in the range $[0,1)$

## Correlated

Points which are "good" in one dimension are also good in all the other dimensions

### Steps

1. select a plane perpendicular to the line from (0,...,0) to (1,...,1) using a normal distribution
2. The normal distribution is used so that more points are in the middle than at the ends
3. Within this plane, individual attribute values are generated using a normal distribution


Working this out for 2 dimensions


## Anti-correlated

Points which are good in one dimension are bad in one or all other dimensions. 

### Steps

1. Select plane perpendiculr to the line from $(0,...,0)$ to $(1, ..., 1)$ using a normal distribution of very small radius so most plans are close to point $(0.5,...,0.5)$
2. within the plane, individual attribute values are picked using a uniform distribution


 ```chart
type: line
labels: [0, 1]
series:
  - title:
    data: [0,1]
tension: 0
width: 80%
labelColors: false
fill: false
```


$$
y = mx + c
$$

In this case, $c= 0$ and $m = 1$. For argument's sake let us say we have picked a point $a$ on this line, the what is $c$ in the plane perpendicular to this line ?

Equation: $y = -x + c'$, this plane passes through $(a, a)$, therefore the line is 
$a = -a + c \rightarrow c = 2a$

$y = -x + 2a$

Now what is the range of these values such that they remain on the plane and within the box ?

If $y = 0, x = 2a; y = 1, x = 2a - 1, x <= 1 and x >= 0$
If $x = 0, y = 2a; x = 1, y = 2a - 1, y <= 1 and y >= 0$

$a \in [0, 1]$

you cannot just bound the ranges, you also have to use the constraints. The generated point has to be on this plane

okay so this is how it's done, vector perp to a plane

vector = (1,...,1)
passes through (a, ..., a)

$$1(a_1 - a) + ... + 1(a_d - a) = 0$$
$$a_1 + a_2 + a_3 + ... + a_d = da$$

we use the null space of $a$ to decide if it is 

```
plane = normal(0.5,0.1)
null_space = null(ones^D)
new_point = point + null_space * uniform((d-1, 1))
check new_point
```

```
plane = normal(0.5,0.3)
null_space = null(ones^D)
if plane <= 0.5
	min = plane
else
	min = 1 - plane
new_point = point + null_space * uniform((d-1, 1))
check new_point
```