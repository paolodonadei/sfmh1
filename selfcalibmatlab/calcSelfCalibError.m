 function [err  ] = calcSelfCalibError(answer,ks)

K1=ks{1,1};
K2=ks{1,2};

trueF1=(abs(K1(1,1))+ abs(K1(2,2)))/2 ;% I have to averaghe this cuz our methods dont predict AR

trueF2=(abs(K2(1,1))+ abs(K2(2,2)))/2; % I have to averaghe this cuz our methods dont predict AR

f1err=abs(answer(1,1)-trueF1);
f2err=abs(answer(1,2)-trueF2);

err=(f1err+f2err);

if(err>1000)
    err=1000;
end

end