function [kk] = convertLinetoMats(pp,framesconstant, numparams,numFrames,w,h)


for i=1:numFrames
    
    
    
    if(framesconstant==1)
        j=1;
    else
        j=i;
    end
    
    
    kk{i,1}=eye(3);
    
    if(numparams>0)
        kk{i,1}(1,1)=pp(((j-1)*numparams)+1,1);
        kk{i,1}(2,2)=pp(((j-1)*numparams)+1,1);
        kk{i,1}(1,3)=w/2;
        kk{i,1}(2,3)=h/2;
        
    end
    
    
    if(numparams>1)
        kk{i,1}(1,3)=pp(((j-1)*numparams)+2,1);
    end
    
    
    if(numparams>2)
        kk{i,1}(2,3)=pp(((j-1)*numparams)+3,1);
    end
    
    
    
    if(numparams>3)
        kk{i,1}(2,2)=kk{i,1}(1,1)*pp(((j-1)*numparams)+4,1);
    end
    
    
    if(numparams>4)
        kk{i,1}(1,2)=pp(((j-1)*numparams)+5,1);
    end
    
    
    
end

end
