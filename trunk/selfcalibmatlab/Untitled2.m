clear all
close all
clc

numtries=100;
numts=100;
initialT= 0.0001 ;
tstep= 0.001 ;

errorvals=zeros(numtries,numts);
current_errors_XY=zeros(numtries,numts);



for i=1:numts
    currentT=initialT + ((i-1)*tstep);
    t(1,i)=currentT;
    for j=1:numtries
        numbad=ceil(rand()*10);
        numnoise=rand()*0.6;
        [ F, ks ] = generateF( 0,0,1.01,50, 1 ,5, numnoise,numbad);
        [fcl, centerloc] = S2nonlinsolveEssNframdiagnostics(F,512,512,currentT) ;
        [errorvals(j,i), current_errors_XY(j,i)  ] = calcSelfCalibError(fcl, centerloc,ks);
        disp([' i is ' num2str(i) ' and j is ' num2str(j)]);
    end
    
    
    
end

plot(t,mean(errorvals));

figure

plot(t,mean(current_errors_XY));