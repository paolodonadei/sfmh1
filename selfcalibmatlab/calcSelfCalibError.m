function [errf, errOC  ] = calcSelfCalibError(answerF,answerloc,ks)

K1=ks{1,1};
K2=ks{1,2};

[m,n]=size(ks);

trueF1=0;
trueF2=0;
trueX=0;
trueY=0;
for i=1:n
    trueF1=trueF1+abs(ks{1,i}(1,1));
    trueF2=trueF2+abs(ks{1,i}(2,2));

    trueX= trueX+abs(ks{1,i}(1,3));
    trueY=trueY+abs(ks{1,i}(2,3));
end


trueF1=trueF1/n;
trueF2=trueF2/n;

trueX= trueX/n;
trueY=trueY/n;


f1err=(abs(answerF(1,1)-trueF1)/trueF1);
f2err=(abs(answerF(1,2)-trueF2)/trueF2);

xerror=(abs(trueX-answerloc(1,1)))/trueX;
yerror=(abs(trueY-answerloc(1,2)))/trueY;

errf=((f1err+f2err)/2)*100;

errOC=((xerror+yerror)/2)*100;

if(errf>100)
    errf=100;
end

if(errOC>100)
    errOC=100;
end

end