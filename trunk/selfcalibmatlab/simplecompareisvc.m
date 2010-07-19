clear all; 
clc;
close all;
seqname= 'wadham'

numframz=5;
w=1024;
h=768;

[corrs, IMS, P,K, F, E]  = readCorrsOxford(seqname,0, 0,numframz);

f=0;
count=0;
for i=1:numframz
    count=count+1;
    f=f+((K{1,i}(1,1)+K{1,i}(2,2))/2);
end
f=f/count;



f

fcl1_40=1164.612443;
x= 1128.383331;

[fcl, centerloc] = S2nonlinsolveEssNframestimator(F,w,h);
[ x, centerloc ] = PeterSturmSelfRobust( F,w,h );
errornonlin=((abs(fcl1_40(1,1)-f))/f)*100;

errpeter=((abs(x(1,1)-f))/f)*100;

errornonlin

errpeter