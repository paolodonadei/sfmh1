function y = monteCarloSampling(s, n, pvi)

[m,msize]=size(pvi);

if(n~=msize)
    display('incompatible msizes');
end

if(msize<=s)
      display(' not enough correspondence pvis');
    return
end
continumm=zeros(msize+1);

continumm(1)=0;
for i=1:msize
    continumm(i+1)=continumm(i)+pvi(i);
end

if(continumm(msize+1)<eps)
   display(' pvi array seems to be empty');
    return
end
collected=0;
y=zeros(s,1);

while collected<s
    curRandval=rand*continumm(msize+1);
    
    curCandidate=0;
    for j=1:msize
        if(curRandval> continumm(j) && curRandval< continumm(j+1));
            curCandidate=j;
            break;
        end
    end
    
    for i=1:s
        if(y(i)==curCandidate)
            curCandidate=0;
            break;
        end
    end
    
    if(curCandidate>eps)
        collected=collected+1;
        y(collected)=curCandidate;
    end
end

        
    
    
    
    
    
end