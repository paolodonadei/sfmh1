function [pvi] = calc_initial_pvi_leverage(x1, x2)


[L] = calc_leveragefromCorrs(x1, x2)

minh=min(h);
maxh=max(h);
rangeh=maxh-minh;
hh=(h-minh)*(1/rangeh);
pvi=1-hh;



end