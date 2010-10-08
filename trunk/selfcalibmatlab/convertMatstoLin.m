function [pp] = convertMatstoLin(kk,framesconstant, numparams,numFrames,w,h)

if(framesconstant==1)
    maxloop=1;
else
    maxloop=numFrames;
end

for i=1:maxloop
    
    if(numparams>0)
        pp(((i-1)*numparams)+1,1)=kk{i,1}(1,1);
    end
    
    
    if(numparams>1)
        pp(((i-1)*numparams)+2,1)=kk{i,1}(1,3);
    end
    
    
    if(numparams>2)
        pp(((i-1)*numparams)+3,1)=kk{i,1}(2,3);
    end
    
    
    
    if(numparams>3)
        pp(((i-1)*numparams)+4,1)=kk{i,1}(2,2)/kk{i,1}(1,1);
    end
    
    
    if(numparams>4)
        pp(((i-1)*numparams)+5,1)=kk{i,1}(1,2);
    end
    
    
    
end

end
