# PBR/Math Notes

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

### Limits
Sometimes we want to know what a value could be if it seems impossible to calculate.
So, math thought of a way to, by cheating a little, get the value anyway named limits.

As an example, we'll take the function
$$f(x)=\frac{x^2-1}{x-1}$$
As you can probably notice, $x=0$ is undefined because you would be dividing by zero.

So, we can try to guess the value anyway by taking a limit. To calculate it algebraically:
$$\lim_{x\rightarrow1}f(x)=\lim_{x\rightarrow1}\frac{x^2-1}{x-1}$$
$$=\lim_{x\rightarrow1}\frac{(x+1)(x-1)}{x-1}$$
Now comes the cheat. You can cross out the $x-1$ and be left with your answer
$$=\lim_{x\rightarrow1}x+1=2$$

Another use-case if when you want to know the value the function converges to
(what value the function will have at $\infty$ or $-\infty$).  
In this case, you may devide all factors by the highest power of $x$ and use the simple rule
$$\lim_{x\rightarrow\infty}\frac{a}{x}=0$$

So, to take the function 
$$f(x)=\frac{2x-1}{x}$$
the funciton converges to
$$\lim_{x\rightarrow\infty}f(x)=\lim_{x\rightarrow\infty}\frac{2x-1}{x}$$
$$=\lim_{x\rightarrow\infty}\frac{2-\frac{1}{x}}{1}$$
$$=\lim_{x\rightarrow\infty}\frac{2-0}{1}=2$$

### Deriving
A derivative is nothing more than a change in a very tiny area. To put this in math:
$$f'(x)=\lim_{h\rightarrow0}\frac{f(x+h)-f(x)}{h}$$
To derive any function of a basic form $f(x)=ax^n$, the derivative is
$$f'(x)=an\times x^{n-1}$$
or
$$\frac{\mathrm{d}y}{\mathrm{d}x}=\frac{\mathrm{d}}{\mathrm{d}x}f(x)=an\times x^{n-1}$$

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

To compute the partial derivative with respect to $x$ for example, you assume that $y$ is constant.
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
where $p(x)\ne0\ \forall\ |f(x)|\gt0$. ($p(x)$ is nonzero for all values of $f$ not on the x-axis)

## Radiometry, spectra and color
### Basic quantities
As a starting point, there is energy based on the wavelength $\lambda$ of the light
$$Q=\frac{hc}{\lambda}$$
where $c=2.99792458\times10^8ms^{-1}$ and $h=6.62606957\times10^{-34}Js$

To get the wattage from this energy, you can use the formula for _radiant flux_ or _power_
$$\Phi=\lim_{\Delta t\rightarrow0}\frac{\Delta Q}{\Delta t}=\frac{\mathrm{d}Q}{\mathrm{d}t}$$

With a unit of Joules per second (J/s) or watts (W)

To measure the density of power or the _irradiance_ at a point, you can use the formula
$$E=\frac{\Phi\cos\theta}{A}$$
for a sphere that is
$$E=\frac{\Phi\cos\theta}{4\pi r^2}$$

To get the _intensity_ of the light, you divide the flux by the amount of directions $\omega$.
For example, for a sphere this is $4\pi$. More generally, the formula is
$$I=\lim_{\Delta\omega\rightarrow0}\frac{\Delta\Phi}{\Delta\omega}=\frac{\mathrm{d}\Phi}{\mathrm{d}\omega}$$

The most important quantity is the _radiance_ $L$. Irradiance and radiant exitance give us differential power 
per differential area at a point $p$, but they do not distinguish the directional distribution of power. 
Radiance takes this last step and measures irradiance or radiant exitance with respect to solid angles. 
It is defined by
$$L(p,\omega)=\lim_{\Delta\omega\rightarrow0}\frac{\Delta E_\omega(p)}{\Delta\omega}=\frac{\mathrm{d}E_\omega(p)}{\mathrm{d}\omega}$$
or
$$L=\frac{\mathrm{d}^2\Phi}{\mathrm{d}\omega\mathrm{d}A^\perp}$$
where $A^\perp$ is the projected area on a hypothetical perpendicular surface to $\omega$.

### Radiometric integrals
The irradiance of a point $p$ on a set of directions $\Omega$ is
$$E(p,n)=\int_\Omega L_i(p,\omega)|\cos\theta|\mathrm{d}\omega$$

To take the integral over spherical coordinates, $\omega=\sin\theta\ \mathrm{d}\theta\mathrm{d}\phi$.
When doing this on a hemisphere $\Omega=H^2(n)$, the irradiance is
$$E(p,n)=\int_0^{2\pi}\int_0^{\frac{1}{2}\pi}L_i(p,\theta,\phi)\cos\theta\sin\theta\ \mathrm{d}\theta\ \mathrm{d}\phi$$
or
$$E=\pi L_i$$

To take the integral over area
$$\mathrm{d}\omega=\frac{\mathrm{d}A\cos\theta}{r^2}$$
where $\theta$ is the angle between the surface normal of $A$ and the vector to $p$. So,
$$E(p,n)=\int_AL\cos\theta_i\frac{\cos\theta_o\mathrm{d}A}{r^2}$$

<img style="background-color:white" alt="Projection of an area" src="https://www.pbr-book.org/4ed/Radiometry,_Spectra,_and_Color/pha04f09.svg">

### Surface reflection
The constant of proportionality defines the surface’s BRDF $f_r$ for the particular pair of directions $\omega_i$ and $\omega_o$:
$$f_r(p, \omega_o,\omega_i)=\frac{\mathrm{d}L_o(p,\omega_o)}{\mathrm{d}E(p,\omega_i)}=\frac{\mathrm{d}L_o(p,\omega_o)}{L_i(p,\omega_i)\cos\theta_i\ \mathrm{d}\omega_i}$$

The basic rules of PBR also state the following:
- _Reciprocity_: The result of the function must be the same even if $\omega_o$ and $\omega_i$ are flipped.
- _Conservation of energy_: The total energy reflected cannot exceed the total incoming energy.

The _hemispherical-directional reflectance_ is a 2D function that gives the total reflection in a given direction due to constant illumination over the hemisphere, or, 
equivalently, the total reflection over the hemisphere due to light from a given direction. It is defined as

$$\rho_{hd}(\omega_o)=\int_{H^2(n)}f_r(p,\omega_o,\omega_i)|\cos\theta_i|\mathrm{d}\omega_i$$

The hemispherical-hemispherical reflectance of a BRDF, denoted by $\rho_{hh}$, gives the fraction of incident light reflected by a surface when the incident light is the same 
from all directions. It is
$$\rho_{hh}=\frac{1}{\pi}\int_{H^2(n)}\int_{H^2(n)}f_r(p,\omega_o,\omega_i)|\cos\theta_o\cos\theta_i|\mathrm{d}\omega_o\ \mathrm{d}\omega_i$$

A BTDF defines the transmission of light. This does not need to abide to the rule of reciprocity.

Both the BRDF and the BTDF combined is the BSDF, whose radiance function is
$$L_o(p,\omega_o)=\int_{S^2}f(p,\omega_o,\omega_i)L_i(p,\omega_i)|\cos\theta_i|\mathrm{d}\omega_i$$
When the spherical domain $S^2$ is used, this is called the _scattering function_. In the case that
the hemispherical domain $H^2(n)$ is chosen, it is called the _reflection equation_.

### Emittance
An object emits light based on its temperature. To get the emittance of a specific wavelength $\lambda$
you can use Planck's law, which is
$$L_e(\lambda,T)=\frac{2hc^2}{\lambda^5(e^{hc/\lambda k_bT}-1)}$$
where $k_b=1.3806488\times10^{-23}J/K$.

<img style="background-color:white" alt="Reflection distributions" src="https://www.pbr-book.org/4ed/Radiometry,_Spectra,_and_Color/pha04f12.svg">

_The emittance of a blackbody emitter at different temperatures._

Non-blackbody emitters follow Kirchhoff's law, which is
$$L_e'(\lambda,\omega,T)=L_e(\lambda,T)(1-\rho_{hd}(\omega))$$

Stefan-Boltzmann gives a law for the radiant exitance at a point $p$:
$$M(p)=\sigma T^4$$

To get the color temperature (the wavelength with the highest strength), you can use
$$\lambda_{max}=\frac{k_w}{T}$$
where $k_w=2.8977721\times10^{-3}mK$.

## Reflection models
### Basics
A _bidirectional scattering distribution function_ (**BSDF**) shows how the light is scattered
from a surface.
This is split into the _bidirectional reflectance distribution function_ (**BRDF**) and the
_bidirectional transmission distribution function_ (**BTDF**)
When calculating light, the BSDF takes in an incoming direction $\omega_o$ and a light direction $\omega_i$
which both point **away** from the surface.

<img style="background-color:white" alt="Reflection distributions" src="https://www.pbr-book.org/4ed/Reflection_Models%2Fpha09f01.svg">  

The image shows the distribution of reflected light possible with (a) diffuse, (b) specular glossy,
(c) perfect specular and (d) retro-reflective from an incident direction $\omega_o$.

All BSDF types will have the following functions implemented:
- `Spectum f(vec3 wo, vec3 wi)`: A function that returns the value of the distribution
- `float PDF(vec3 wo, vec3 wi)`: Calculates the value of the PDF

Some may have:
- `float D(vec3 w)`: The microfacet distribution function
- `float G1(vec3 w, vec3 wm)`: The microfacet _masking function_
- `float G(vec3 wo, vec3 wi)`: The microfacet _masking-shadowing function_

### Coordinate system
<img style="background-color:white" alt="Coordinate system" src="https://www.pbr-book.org/4ed/Reflection_Models/pha09f02.svg">  

The directions $\omega_o$ and $\omega_i$ will be converted to the surface coordinate system
when calculating the light.

So, the coordinate system will be defined by the vectors $(\vec{s},\vec{n},\vec{t})$ for $(x,y,z)$

### Diffuse
This is the easiest one of them all. It simply returns the color itself, but times $\frac{1}{\pi}$
because of the hemisphere reflection.  
So, `Spectrum f(vec3 wo, vec3 wi) { return material.diffuseColor * INVPI }`  
The PDF only returns something if the surface needs to reflect. So,
```glsl
float PDF(vec3 wo, vec3 wi) 
{ 
    if (wo.y * wi.y > 0.0)
        return 0.0;
    return abs(wo.y) * INVPI;
}
```

## To read: Ch. 2.2-2.4, 4.3.2, 4.4.2, 4.6, 5, 6.3-6.8, 8, 9.1.3-9.1.4, 9.3-9.9, 10-16