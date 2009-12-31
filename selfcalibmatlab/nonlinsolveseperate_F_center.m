function [fcl, centerloc] = nonlinsolveseperate_F_center(TF)


fcl=[0 0];
xcen=0;
ycen =0;
centerloc=[xcen ycen];
x = PeterSturmSelf( TF );

if(x(1,1)>200 && x(1,1)<1600)
    finit=x(1,1);
else
    finit=512; %w+h
end
xinit=256;
yinit=256;

options  =optimset('Display','off','Jacobian','off','Algorithm','levenberg-marquardt');

for i=1:200
    
    
    f_centrfixed = @(x)computerEssentialErrorcenterfixed(x,TF,xinit,yinit);
    
    [x,fval,exitflag,output]  = fsolve(f_centrfixed ,[finit],options);
    
    
    
end
