function z=randsampleengine(s, n, pvi,typenum)
if(typenum==1)
    z= randsample(s, n);
elseif(typenum==2)
    z = monteCarloSampling(s, n, pvi);
else
    display('wrong item');

end
end