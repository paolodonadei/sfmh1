function y = monteCarloSampling(s, n, pvi)

[msize,m]=size(pvi);

if(s~=msize)
    display('incompatible msizes');
end

if(msize<=n)
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
    pvi=0.5*ones(msize,1);
    
    continumm=zeros(msize+1);
    
    continumm(1)=0;
    for i=1:msize
        continumm(i+1)=continumm(i)+pvi(i);
    end
    
end
collected=0;
y=zeros(n,1);

trials=0;

while collected<n
    curRandval=rand*continumm(msize+1);

    trials=trials+1;
    
    if(trials>(n*100))
        pvi
        display('cant seem to get enough ppoints');
        y=round(rand(n,1)*msize);
        y(find(y==0))=1;
        return;
    end
    curCandidate=0;
    for j=1:msize
        if(curRandval> continumm(j) && curRandval< continumm(j+1));
            curCandidate=j;
            break;
        end
    end
    
    for i=1:n
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