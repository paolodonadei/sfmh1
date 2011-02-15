function z=randsampleengine(s, n, pvi,typenum)
if(typenum==1 || typenum==2)
    z= randsample(s, n);
else
    z = monteCarloSampling(s, n, pvi);
end
end