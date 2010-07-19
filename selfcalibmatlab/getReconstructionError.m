function [er] = getReconstructionError(P1,P2,corrs)
er=0;

   x1=corrs{1,1};
   x2= corrs{2,1};


for i=1:(size(x1,2))
  
    xc1=x1(:,i);
    xc2=x2(:,i);
    stru=trangulate(P1,P2,xc1,xc2);
    X(1,1)=stru(1,1);
    X(2,1)=stru(2,1);
    X(3,1)=stru(3,1);
        X(4,1)=1;
    
    xp1=P1*X;
    xp2=P2*X;
    
    xp1=xp1/xp1(3,1);
    xp2=xp2/xp2(3,1);
    
    xpe1=xp1-xc1;
    
    xpe2=xp2-xc2;

    errc=sum((xpe1.^2)+(xpe2.^2));

    er=er+errc;
    
    
end


er=er/size(x1,2);




end