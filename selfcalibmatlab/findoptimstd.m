clear all
close all
clc

numtries=1000;


errorinlier=zeros(numtries,1);
erroroutlier=zeros(numtries,1);




for j=1:numtries
    numbad=ceil(rand()*7);
    numnoise=rand()*0.6;
    [ F, ks ] = generateF( 0,0,1.01,50, 1 ,5, numnoise,numbad);
    disp([' j is ' num2str(j) ]);

    solution=[ks{1,1}(1,1)+(randn()*50) ks{1,1}(1,3)+(randn()*20) ks{1,1}(2,3)+(randn()*20) ks{1,1}(2,2)+(randn()*50)];



    erroroutlier(j,1)=computerEssentialErrorSVDNFramesWeighted(solution,F(1:numbad));
    errorinlier(j,1)=computerEssentialErrorSVDNFramesWeighted(solution,F(numbad+1:10));

end



