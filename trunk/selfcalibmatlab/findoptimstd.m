clear all
close all
clc

numtries=100;
numts=10;
initialT= 0.001 ;
tstep= 0.005 ;

errorvals=zeros(numtries,numts);
current_errors_XY=zeros(numtries,numts);



for i=1:numts
    
    for j=1:numtries
        currentT=initialT + ((i-1)*tstep);
        [ F, ks ] = generateF( 0,0,1.01,50, 1 ,5,0.1,5);
        [fcl, centerloc] = S2nonlinsolveEssNframdiagnostics(F,512,512,currentT) ;
        [errorvals(j,i), current_errors_XY(j,i)  ] = calcSelfCalibError(fcl, centerloc,ks);
    end
    
    
    
end