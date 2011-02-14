function z=randsampleengine(s, n, pvi,typenum)
if(typenum==1 || typenum==2)
    z= randsample(s, n);
elseif(typenum==3 || typenum==4 || typenum==5)
    y = monteCarloSampling(s, n, pvi);
end
end