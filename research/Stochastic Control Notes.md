
### Stochastic Optimal Control with Applications to Market Making

We review the fundamentals of the theory of stochastic optimal control and its application to market making in limit order books. 

The usual setup is to work with controlled diffusion processes of the form, 

$$
dX_s = b(X_s, \alpha_s)ds + \sigma(X_s, \alpha_s)dW_s
$$

#### Merton's Problem

For example, in the classic Merton's Problem, we have the following, 

 - A stock price $S_t$ which follows : $dS_t = \mu S_t dt + \sigma S_t dW_t$.
 - An agent trades in the stock and cash, holding $H_t$ shares of the stock at time $t$.
 - The cash in the portfolio is $X_t - H_tS_t$.
 - The wealth process $X_t$ has the dynamics, 
 
   $$
   \begin{align}
   dX_t &= H_t dS_t + r\left(X_t - H_tS_t\right) dt \\
   &= H_t\left(\mu S_t dt + \sigma S_t dW_t\right)+ r\left(X_t - H_tS_t\right) dt\\
   &= rX_t dt +H_t \left[ (\mu-r) S_t dt + \sigma S_t dW_t\right]\\
   &= rX_t dt +H_t S_t\left[ (\mu-r) dt + \sigma dW_t\right]\\
   &= rX_t dt +\underbrace{H_t S_t}_{\pi_t} \sigma\left[ \underbrace{\frac{(\mu-r)}{\sigma}}_{\lambda} dt +
   dW_t\right]\\
   &= rX_t dt +\pi_t \sigma\left[ \lambda dt + dW_t\right]
   \end{align}
   $$
   
   where in the above, $\pi_t$ is the control and $\lambda$ is the market price of risk for the stock.
 - The finite horizon problem is posed as having the agent maximise his expected utility of terminal wealth over the horizon $\left[0,T\right]$. The value function for the agent is, 
 
   $$
   u(x):= \underbrace{\sup}_{\pi \in \mathcal{A}(x)} \mathbb{E}\left[\left.U(X_T)\right| X_0 = x\right]
   $$
   
   where $x$ is the initial wealth and $U(\cdot)$ is some utility function.

#### The General Setup

In general, the value function is of the form, 

$$
u(x):= \sup_{\pi \in \mathcal{A}(x)} \mathbb{E}\left[\left.\int_0^T f(s, X_s, \pi_S)ds + F(X_T) 
\right|X_0=x\right]
$$

Why is the value function defined as such? 

 1. We can think of $F(\cdot)$ as quantifying the value of the terminal wealth $X_T$. 
 2. For $f(\cdot, \cdot, \cdot)$, we can think of it as quantifying the "instantaneous" utility at every time instance, and the "full reward" for carrying out our strategy is obtained by integrating $f(\cdot, \cdot, \cdot)$ from $0$ to $T$. 
 3. Of course, since $X_T$ is a stochastic quantity, naturally we would take expectations.
 4. Finally, we take the supremum (over the set of admissible strategies) of the expectation.
 
For example, in Merton's Utility from Terminal Wealth and Intermediate Consumption Problem, the value function is, 

$$
u(x):= \sup_{(\pi, c) \in \mathcal{A}(x)} \mathbb{E}\left[\left.\int_0^T e^{-\delta t}U_1(c_t) ds + U_2(X_T) 
\right|X_0=x\right]
$$

where $c_t$ is consumption at time $t$ and $U_1, U_2$ are utility functions. 

So what the above intuitively says is that, we want to chose optimal controls $(\pi, c)$ to maximize the expected utility of, 

 1. Our sum of utility from consumption across time from $0$ to $t$, given by $\int_0^T e^{-\delta t}U_1(c_t) ds$.
 2. Our utility of terminal wealth $X_T$, given by $U_2(X_T)$.

#### Dynamic Programming and the Hamilton-Jacobi-Bellman Equation

We now explore how to go about solving control problems such as the above using the dynamic programming formulation and explore its link to HJB equations.

As usual, we work with controlled diffusions of the form, 

$$
dX_s = b(X_s, \alpha_s)ds + \sigma(X_s, \alpha_s)dW_s
$$

And we're interested in the **finite horizon problem**, 

$$
u(x):= \sup_{\alpha \in \mathcal{A}(x)} \mathbb{E}\left[\left.\int_0^T f(s, X_s, \alpha_S)ds + F(X_T) 
\right|X_0=x\right]
$$

where $T \in (0, \infty)$, $\mathcal{A}(x)$ is the set of admissible controls, given $X_0=x$.

The dynamic programming formulation tackles the problem by considering a starting state $(t, x)$, and let  $\mathcal{A}(t,x)$ be the set of admissible controls with this starting state.

A new notation is also introduced where we will write $\left(X_s^{t, x}\right)_{s\in (t,T)}$ to denote a path of the controlled diffusion starting at $X_t=x$.

To cater to different starting points, we define an objective functional, 

$$
J(t, x;\alpha) := \mathbb{E}\left[\left.\int_t^T f(s, X_s^{t,x}, \alpha_S)ds + F(X_T^{t,x}) 
\right|X_t=x\right]
$$

The objective functional states that, if we start from time $t$, with initial value of $X_t=x$, and apply the control $\alpha$ (any arbitrary control), the expectation of the terminal reward $F(X_T^{t,x})$ and the ongoing sum of rewards $\int_t^T f(s, X_s^{t,x}, \alpha_S)ds$, is defined as $J(t, x;\alpha)$.

With that we can write the value function starting at time $t$ as,

$$
u(t, x) := \sup_{\alpha \in \mathcal{A}(t,x)} J(t, x;\alpha)
$$

and using this notation, we can write the original value function starting at $t=0$ as, 

$$
u(x) \equiv u(0,x)
$$

So far, we have only been introducing additional notation to facilitate the formulation of the problem as a dynamic programming problem.

#### The Dynamic Programming Principle

In this section we introduce the dynamic programming principle, which will lead us to the HJB equation involving a PDE on $u(t,x)$. Solving the PDE and maximising it over the space of admissible controls would give us the optimal control to use.

The dynamic programming principle starts of with comparing two strategies, 

 - $\mathbf{I}$ : Using the optimal control $(\hat{\alpha}_s)_{s\in[t, T]}$ over the interval $[t,T]$, versus :
 - $\mathbf{II}$ : Using an arbitrary control $(\alpha_s)_{s\in[t, t+h)}$ over the interval $[t,t+h]$ (where $h$ is a small time interval), and then using the optimal control $(\hat{\alpha}_s)_{s\in[t+h, T]}$.
 
With the above formulation, it is immediately obvious that strategy $\mathbf{I}$ is at least as good (since $\hat{\alpha}_s$ is the optimal control over the whole period) as strategy $\mathbf{II}$. 

Given this, we can write, 

$$
u(t,x) \ge \mathbb{E}\left[\underbrace{\int_t^{t+h} f(s, X_s^{t,x}, \alpha_S)ds}_{\text{running reward from $t$ to $t+h$}} + \underbrace{u(t+h, X_{t+h}^{t,x})}_{\text{the optimal value from $t+h$ to $T$}}
\right]
$$

Note that in the above, the inequality results from the term $\mathbb{E}\left[\int_t^{t+h} f(s, X_s^{t,x}, \alpha_S)ds\right]$, due to the fact that it is using an **arbitrary** control $\alpha_s$.

To get equality, we need to maximize the RHS over the space of controls. This means we must write, 

$$
u(t,x) = \sup_{(\alpha_x)_{s\in[t,t+h)}} \mathbb{E}\left[\underbrace{\int_t^{t+h} f(s, X_s^{t,x}, \alpha_S)ds}_{\text{running reward from $t$ to $t+h$}} + \underbrace{u(t+h, X_{t+h}^{t,x})}_{\text{the optimal value from $t+h$ to $T$}}
\right]
$$

In the next step, we apply Ito on $u(t, x)$, 

$$
\begin{aligned}
du &= u_t dt + u_x dX_t + \frac{1}{2} u_{xx} d\langle X\rangle_t\\
&= u_t dt + u_x \left[b(X_t, \alpha_t)dt + \sigma(X_t, \alpha_t)dW_t\right] + \frac{1}{2} u_{xx} \sigma^2(X_t, \alpha_t)dt \\
&=u_t dt +  b(X_t, \alpha_t)u_x dt + \frac{1}{2} \sigma^2(X_t, \alpha_t)  u_{xx} dt +  \sigma(X_t, \alpha_t)u_xdW_t
\end{aligned}
$$

In integral form, we can also write, 

$$
\begin{aligned}
u(t+h, X_{t+h}^{t,x}) &= u(t, x) + \int_t^{t+h} \frac{\partial u}{\partial t}(s, X_s^{t,x}) + \underbrace{b(X_s, \alpha_s)\frac{\partial u}{\partial x}(s, X_s^{t,x}) + \frac{1}{2} \sigma^2(X_s, \alpha_s) \frac{\partial^2 u}{\partial x^2}(s, X_s^{t,x})}_{\text{this is termed } \mathcal{L}^\alpha(\cdot) \text{, the generator of diffusion}} ds + \int_t^{t+h} \sigma(X_s, \alpha_s)\frac{\partial u}{\partial x}(X_s, \alpha_s)dW_s\\
&=  u(t, x) +\int_t^{t+h}\left( \frac{\partial u}{\partial t} + \mathcal{L}^\alpha u\right )\left(s, X_s^{t,x}\right)ds+ \int_t^{t+h} \sigma(X_s, \alpha_s)\frac{\partial u}{\partial x}(X_s, \alpha_s)dW_s
\end{aligned}
$$

If we substitute $u(t+h, X_{t+h}^{t,x})$ into the inequality involving $u(t,x)$, the stochastic integral would disappear under the expectation operator, and we get, 

$$
\begin{aligned}
&u(t,x) \ge \mathbb{E}\left[\int_t^{t+h} f(s, X_s^{t,x}, \alpha_S)ds + u(t, x) +\int_t^{t+h}\left( \frac{\partial u}{\partial t} + \mathcal{L}^\alpha u\right )\left(s, X_s^{t,x}\right)ds\right]\\
\Rightarrow &\mathbb{E}\left[\int_t^{t+h} f(s, X_s^{t,x}, \alpha_S)ds +\left( \frac{\partial u}{\partial t} + \mathcal{L}^\alpha u\right )\left(s, X_s^{t,x}\right)ds\right]\le 0
\end{aligned}
$$

Now we divide by $h$, move it within the expectation, and let $h$ tends to zero. This gives us, 

$$
\begin{aligned}
f(t, x, \alpha) + \left( \frac{\partial u}{\partial t} + \mathcal{L}^\alpha u\right ) \left(t, x\right) &\le 0\\
\Rightarrow \frac{\partial u}{\partial t} +  \mathcal{L}^\alpha u(t, x)+f(t, x, \alpha) &\le 0 
\end{aligned}
$$

This inequality result is for any **arbitrary** control $\alpha$.


#### The Hamilton-Jacobi-Bellman Equation
To get equality, we must have the optimal $\alpha$, with which we can write the **HJB Equation**, 

$$
\frac{\partial u}{\partial t} + \sup_{\alpha \in \mathcal{A}}\left[ \mathcal{L}^\alpha u(t, x)+f(t, x, \alpha)\right] = 0 
$$

In other words, for the optimal $\alpha=\hat{\alpha}$, we have, 

$$
\frac{\partial u}{\partial t} + \mathcal{L}^\hat{\alpha} u(t, x)+f(t, x, \hat{\alpha}) = 0 
$$

#### Stochastic Control for Counting Process

So far we've worked with diffusions as the driving source of uncertainty in the control problem. In market-making, where-by participants post limit-orders and market-orders, it is also important to incorporate counting process for driving uncertainty in the model.

We will provide a quick introduction to Poisson Counting Processes and see how they fit in to the same dynamic programming framework as we have described above.

##### The Poisson Process

A Poisson process is the continuous time analogue of the Bernoulli process with the following properties, 
 - it is subject to jumps of fixed (or possibly random) size.
 - its mean arrival rate over an interval $dt$ is $\lambda$. Specifically, we define the probability of $k$ arrivals in time $dt$ as
 
   $$
       P(k, dt)=\begin{cases}
               1-\lambda dt, \quad\text{if $k=0$}\\
               \lambda dt, \,\,\,\qquad\text{if $k=1$} \\
               0, \,\quad\qquad\text{if $k>1$} 
            \end{cases}
   $$
   
   and note that in the above, we mean the above holds in the limit as $dt$ goes to zero. The "arrival rate" $\lambda$ is the expected number of arrivals per unit time.
   
   Using the above assumption, we consider the probability that there are $k$ arrivals in time interval $t+\Delta t$, denoted by $P(k, t+dt)$. By the law of total probability, we must have the following relation, 
   
   $$
   \begin{aligned}
   P(k, t+dt) &= \underbrace{P(k, t) }_{\text{$k$ arrivals in time $t$}}\times \underbrace{P(0, dt)}_{\text{0 arrivals in time $dt$}} + \underbrace{P(k-1, t)}_{\text{$k-1$ arrivals in time $t$}} \times \underbrace{P(1, dt)}_{\text{1 arrival in time $dt$}}\\
   &= P(k, t) \times (1-\lambda dt) + P(k-1, t) \times \lambda dt
   \end{aligned}
   $$

The next step is to manipulate the above expression into a differential equation. 

We begin by re-arranging terms, 

$$
\begin{aligned}
P(k, t+dt) &= P(k, t) \times (1-\lambda dt) + P(k-1, t) \times \lambda dt\\
P(k, t+dt) &= P(k, t) - P(k, t) \lambda dt+ P(k-1, t) \lambda dt\\
P(k, t+dt) - P(k,t) &= \lambda \left[P(k-1, t) - P(k, t)\right] dt\\
\end{aligned}
$$

From the above, if we go to the limit as $dt$ tends to zero, 

$$
\begin{aligned}
\frac{\partial P(k, t)}{\partial t} &= \lim_{dt \rightarrow 0} \frac{P(k, t+dt) - P(k,t) }{dt}\\
&= \lambda \left[P(k-1, t) - P(k, t)\right]
\end{aligned}
$$

We now show that the following function satisfy the above differential equation, 

$$
P(k, t) = e^{-\lambda t}\frac{(\lambda t)^k}{k!}
$$



Differentiating wrt $t$ gives, 

$$
\begin{aligned}
\frac{\partial P(k, t)}{\partial t} &= e^{-\lambda t} \frac{\lambda k(\lambda t)^{k-1}}{k!} -\lambda e^{-\lambda t}\frac{(\lambda t)^k}{k!}\\
&= \lambda \left[ e^{-\lambda t} \frac{ k(\lambda t)^{k-1}}{k!} - e^{-\lambda t}\frac{(\lambda t)^k}{k!}\right]\\
&= \lambda \left[ e^{-\lambda t} \frac{ (\lambda t)^{k-1}}{k-1!} - e^{-\lambda t}\frac{(\lambda t)^k}{k!}\right]\\
&= \lambda [P(k-1, t)-P(k,t)]
\end{aligned}
$$

which is as desired.

##### The Poisson Counting Process

The simplest Poisson Process is that with a jump size of 1, which is termed the Poisson Counting Process. 

It is defined as $(N_t)_{t\ge0}$ with, 

 - $N_t \ge0$
 - $N_t$ is an integer
 - if $ s\le t$ then $N_s \le N_t$
 - $\mathbb{P}(N_t = n) = e^{-\lambda t} \frac{(\lambda t)^n}{n!}$, i.e. the probability that $N_t$ is equal to $n$ is given by the Poisson pdf we derived earlier.
 - $\mathbb{E}[N_t] = \lambda t$ and $\mathbb{Var}[N_t] = \lambda t$ (this is standard result from Poisson distribution).
 - Importantly, it also has **stationary independent increments**, i.e. for $ 0\le s\le t$, the following holds,
 
   $\mathbb{P}(N_{t+h} -N_{s+h} = k) = \mathbb{P}(N_t - N_s = k) = e^{-\lambda (t-s)}\frac{(\lambda( t-s))^k}{k!}$ for all $h > 0$.
 - Another useful result is that $N_t - \lambda t$  is a martingale. 
 
 This is very important because remember in the derivation of the HJB equation, we need to make the stochastic integral wrt to the Wiener Process disappear under the expectation operator. We would need to same for the derivation later on involving the Counting Process.
   
 
Since we often need to work with SDEs of stock price involving jump diffusions like the below, 

$$
\frac{dS_t}{S_t} = \mu dt + \sigma dW_t - \delta dN_t 
$$

we need to now define what is meant by $dN_t$. 

Recall that, roughly speaking, the probability of one count in an interval of amplitude $dt$ is of order $dt$ (given by $\lambda dt$ earlier), while the probability of more than one count is of higher order. We can translate this into the rule, 

   $$
   \begin{aligned}
       dN_t&=\begin{cases}
               1, \quad\text{if there is a jump in the time interval $(t, t+dt]$},\\
               0, \quad\text{otherwise},\\
            \end{cases}\\
       &d\langle N\rangle_t = dN_t, \qquad dN_t dt = 0
   \end{aligned}
   $$


##### Ito's Lemma for Jumps

   
We also need Ito's lemma to work with jumps, and it is but a straight forward extension of the usual Ito's lemma with the addition work of adding back the all the jumps which occured in between the limits of integration.

Recall that for a continuous process $dX_s = b(X_s, \alpha_s)ds + \sigma(X_s, \alpha_s)dW_s$, Ito's lemma is as follows, 

$$
\begin{aligned}
df(X_t) &=  f_x dX_t + \frac{1}{2}f_{xx} d\langle X\rangle _t\\
&=  f_x \left[b(\cdot) dt + \sigma(\cdot) dW_t\right] + \frac{1}{2}f_{xx} \sigma^2(\cdot) dt
\end{aligned}
$$

In integral form, we have, 

$$
\begin{aligned}
X_t &= X_0 + \int_0^t f_x b(\cdot) ds + \int_0^t f_x \sigma(\cdot) dW_s + \frac{1}{2}\int_0^t f_{xx} \sigma^2(\cdot) ds
\end{aligned}
$$

Since conceptually, the integral is a sum, and if we add jumps at finite number of points to the process $X_t$, what we need to extend Ito's lemma is simply to just add back the jumps. 

Assume to following notation, 

$$
N_s = N_{s^-} + 1 \quad \text{if} \quad dN_s=1
$$

Then the extended Ito's lemma is, 


$$
\begin{aligned}
X_t &= X_0 + \int_0^t f_x b(\cdot) ds + \int_0^t f_x \sigma(\cdot) dW_s + \frac{1}{2}\int_0^t f_{xx} \sigma^2(\cdot) ds + \sum_{0\le s\le t} \left[f\left(X_s\right)-f\left(X_{s^-}\right)\right]\\
&= X_0 + \int_0^t f_x b(\cdot) ds + \int_0^t f_x \sigma(\cdot) dW_s + \frac{1}{2}\int_0^t f_{xx} \sigma^2(\cdot) ds + \int_0^t f\left(X_s\right)-f\left(X_{s^-}\right) dN_s
\end{aligned}
$$

Therefore, the differential form of Ito's lemma is, 

$$
\begin{aligned}
df(X_t) &=  f_x dX_t + \frac{1}{2}f_{xx} d\langle X\rangle _t + \left[f\left(X_s\right)-f\left(X_{s^-}\right) \right]dN_s\\
\end{aligned}
$$


**Note** : The reason for all the above hard work on Ito's lemma for Counting Process is so that we can approach the problem of market making using HJB equations with jumps. For example, one of the simplest way to model the arrival of market orders is to use counting processes associated with the number of market orders up to time $t$.

##### HJB Formulation for Counting Processes

For the counting process we defined earlier, the assumption was that the intensity, or "arrival rate", is a constant $\lambda$. In actual fact, we can make $\lambda$ a function of time as well, using $\lambda(t)$.
