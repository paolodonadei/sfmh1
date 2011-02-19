function z=randsampleengine(s, n, pvi,typenum)
if(typenum==1 || typenum==2)
    z= randsample(s, n);
elseif(typenum==5)
    med=median(pvi);
    for i=1:s
        if(pvi(i,1)<med) 
            pvi(i,1)=0;
        end
    end
    
     z = monteCarloSampling(s, n, pvi);
else
    z = monteCarloSampling(s, n, pvi);

end
end