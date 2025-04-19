# PBR Notes

## Basic math concepts
### Basics
- Vectors: $\vec{v}$
- Vector length: $\left|\vec{v}\right|$
- Flooring: $\lfloor x\rfloor$
- Ceiling: $\lceil x\rceil$
- N dot L: $\cos\theta$ or $\langle N,L\rangle$ or $N\cdot L$

### Summation
Gives the sum of all results of a formula from the initial value of a counter (bottom)
to an end point (top).
$$\displaystyle\sum_{k=0}^{n}f(k)=f(0)+f(1)+f(2)+\cdots+f(n)$$

### Products
Gives the product of all results of a formula from the initial value of a counter (bottom)
to an end point
$$\displaystyle\prod_{k=0}^{n}f(k)=f(0)\times f(1)\times f(2)\times \cdots\times f(n)$$

### Set theory
Suppose two sets $A=\{1,2,3,4,5\}$ and $B=\{3,4,5,6,7\}$. Basic set theory defines the following
$$1\in A$$
$$6\notin A$$
$$A\cap B=\{3,4,5\}$$
$$A\cup B=\{1,2,3,4,5,6,7\}$$
$$\{1,2,3\}\cap\{4,5,6\}=\varnothing$$

### Probability
Suppose a probability $p$. For the list $X_i\in\{1,2,3,4,5,6\}$, assuming a uniform distribution,
$p_i=\frac{1}{6}$. A given is that $\sum{p_i}=1$.

When working with independent probabilities
$$p=P(X,Y)=P(X)P(Y)$$
If $Y$ is dependent on $X$
$$p=P(X,Y)=P(X)P(Y|X)$$
$$P(Y|X)=\frac{P(X\cap Y)}{P(Y)}$$
where $Y|X$ is the _conditional probability_ of $Y$ given $X$

A particularly important random variable is the canonical uniform random variable, which is written as $\xi$.
It is the _canonical uniform random value_ and it takes on all values in its domain $[0,1)$ (range from 0 to 1, including 0 and excluding 1)
independently and with uniform probability. It is handy for two reasons:
- It is easy to generate a value with this distribution, since all probabilities are equal.
- It can be mapped to a discrete random variable, choosing $X_i$ if
    $$\displaystyle\sum_{j=1}^{i-1}p_j\le \xi \lt\displaystyle\sum_{j=1}^{i}p_j$$
    so, $\xi$ can for example be used to select a random light from a list

### Deriving
To derive any function of a basic form $f(x)=ax^n$, the derivative is
$$f'(x)=an\times x^{n-1}$$
or
$$\frac{\mathrm{d}y}{\mathrm{d}x}=\frac{\mathrm{d}}{\mathrm{d}x}f(x)=n\times x^{n-1}$$

Thus, the derivative of $f(x)=ax+b$ is 
$$f'(x)=\left[ax^1+b\right]'=a$$

A derivative of a constant function $f(x)=a$ is $0$

There are some other special cases, namely
$$[e^x]'=e^x$$
$$[a^x]'=a^x\ln(a)$$
$$[\ln(x)]'=\frac{1}{x}$$
$$[\log_a(x)]'=\frac{1}{x\ln(a)}$$
$$[\sin(x)]'=\cos(x)$$
$$[\cos(x)]'=-\sin(x)$$
$$[\tan(x)]'=\frac{1}{\cos^2(x)}=1+\tan^2(x)$$

There are also some rules, namely
$$(af(x)+bg(x))'=af'(x)+bg'(x)$$
$$(f(x)g(x))'=f'(x)g(x)+g'(x)f(x)$$
$$\left(\frac{f(x)}{g(x)}\right)'=\frac{f'(x)g(x)-g'(x)f(x)}{g(x)^2}$$

#### Partial derivative
When a function has multiple parameters, you can derive only one of them using a partial derivative.
For the function
$$f(x,y)=x^2+xy+y^2$$
the partial derivative for x is written as
$$f_x',\partial_xf,\frac{\partial}{\partial x}f\ \mathrm{or}\ \frac{\partial f}{\partial x}$$

To compute the partial derivative with respect to $x$ for example, you assume that $y$ is constatnt.
So,
$$\frac{\partial}{\partial x}f=2x+1\times y+0=2x+y$$
and same ith respect to $y$
$$\frac{\partial}{\partial y}f=0+x\times1+2y=x+2y$$

### Integration
To integrate the function
$$f(x)=\frac{1}{x}+cos(x)+x^2+1$$
you take the inverse derivative or more commonly named the _antiderivative_.

A general rule for a formula of the form $f(x)=ax^n$
$$F(x)=\int f(x)\mathrm{d}x=\frac{a}{n+1}x^{n+1}$$

We know that
$$\left[\ln(x)\right]'=\frac{1}{x}\Rightarrow\int\frac{1}{x}\mathrm{d}x=\ln(x)$$
$$\left[\sin(x)\right]'=\cos(x)\Rightarrow\int\cos(x)\mathrm{d}x=\sin(x)$$
$$\left[\frac{1}{3}x^3\right]'=x^2\Rightarrow\int x^2\mathrm{d}x=\frac{1}{3}x^3$$
$$\left[x\right]'=1\Rightarrow\int 1\mathrm{d}x=x$$

Thus, the integral of the function $f(x)=\frac{1}{x}+cos(x)+x^2+1$ is
$$\int f(x)\mathrm{d}x=\ln(x)+\sin(x)+\frac{1}{3}x^3+x$$

This result is the primitive function of $f(x)$, which is written as $F(x)$.

To take the integral in a specific range, you simply do
$$\displaystyle\int_a^b f(x)\mathrm{d}x=F(b)-F(a)$$

Thw following is also true
$$\int af(x)\mathrm{d}x=a\int f(x)\mathrm{d}x$$

## Monte Carlo integration
### CDF
The _cumulative distribution function_ (**CDF**) $P(x)$ of a random variable is 
the probability of taking a value in a list that is less than or equal to $x$
$$P(x)=Pr\{X\le x\}$$

For the range $A=\{1,2,3,4,5,6\}$, $P(2)=\frac{1}{3}$.

### Continuous random functions
_Continuous random values_ goes beyond $\xi$ and takes on values over ranges of domains.
For example, the probability of $x$ on a random value that ranges over all real numbers 
from 0 to 2 is $2-x$

### PDF
A _probability density function_ (**PDF**) is simply the derivative of the CDF. The PDF
$$p(x)=\frac{\mathrm{d}P(x)}{\mathrm{d}x}$$

For $\xi$, the PDF is
$$p(x)=\left\{\begin{array}{rcl}1 & x\in[0,1)\\0 & otherwise\end{array}\right.$$

To get the probability that $x$ lies in a specific domain, 
you simply integrate the PDF over that domain
$$Pr\{x\in[a,b]\}=\int_a^b p(x)\mathrm{d}x=P(b)-P(a)$$

### Expected values
To get the _expected value_ $E_p[f(x)]$ over the domain $D$ is defined as
$$E_p[f(x)]=\int_D f(x)p(x)\mathrm{d}x$$

For example, in the range from $0$ to $\pi$ where $p$ is uniform, the expected value is
$$E[\cos(x)]=\int_0^\pi \cos(x)\times\frac{1}{\pi}\mathrm{d}x$$
$$=\frac{1}{\pi}\int_0^\pi \cos(x)\mathrm{d}x$$
$$=\frac{1}{\pi}\left(\sin(\pi)-\sin(0)\right)=0$$

The expected values also has a couple of useful properties:
$$E[af(x)]=aE[f(x)]$$
$$E\left[\displaystyle\sum_{i=1}^nf(X_i)\right]=\displaystyle\sum_{i=1}^nE[f(X_i)]$$

### Monte Carlo estimator
To approximate the value $X_i\in[a,b]$, the Monte Carlo estimator says that
$$F_n=\frac{b-a}{n}\displaystyle\sum_{i=1}^nf(X_i)$$
and that
$$E[F_n]=\int_a^bf(x)\mathrm{d}x$$

To give a more general form:
$$F_n=\frac{1}{n}\displaystyle\sum_{i=1}^n\frac{f(x)}{p(x)}$$
where $p(x)\ne0\ \forall\ |f(x)|\gt0$ ($p(x)$ is nonzero for all values of $f$ not on the x-axis)
