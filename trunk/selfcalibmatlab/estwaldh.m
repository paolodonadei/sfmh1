function h = estwaldh(nep, ep, del)

a = del / ep;
b = (1-del)/(1-ep);

al = log(a);
be = log(b);

x0 = log(1/(1-nep))/be;
v0 = nep * exp(x0 *al);
x1 = log((1-2*v0)/(1-nep))/be;

v1 = nep * exp(x1 * al) + (1-nep) * exp(x1 * be);
h = x0 - (x0 - x1)/(1+v0 - v1)*v0;