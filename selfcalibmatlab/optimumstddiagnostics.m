clear all
clc
close all
count=0;

residualReal=zeros(11,100);
residualSynth=zeros(11,100);

residualReal_gaussian=zeros(11,100);
residualSynth_gaussian=zeros(11,100);

for i=0:0.1:1
    count=count+1;
    for j=1:100
        [ F_S, ks_S ] = generateF( 0, 0,1.01,40,1,5,i,10   );
        [corrs, IMS, P,ks_R, F_R] = readCorrsOxford('wadham', i,10);

        solution_s=[ks_S{1,1}(1,1) ks_S{1,1}(1,3) ks_S{1,1}(2,3) ks_S{1,1}(2,2)];
        solution_s_gaussian=[ks_S{1,1}(1,1)+(randn()*50) ks_S{1,1}(1,3)+(randn()*10) ks_S{1,1}(2,3)+(randn()*10) ks_S{1,1}(2,2)+(randn()*50)];


        solution_r=[ks_R{1,1}(1,1) ks_R{1,1}(1,3) ks_R{1,1}(2,3) ks_R{1,1}(2,2)];
        solution_r_gaussian=[ks_R{1,1}(1,1)+(randn()*50) ks_R{1,1}(1,3)+(randn()*10) ks_R{1,1}(2,3)+(randn()*10) ks_R{1,1}(2,2)+(randn()*50)];


        residualReal(count,j)=  computerEssentialErrorSVDNFramesWeighted(solution_r,F_R);
        residualSynth(count,j)=  computerEssentialErrorSVDNFramesWeighted(solution_s,F_S);

        residualReal_gaussian(count,j)= computerEssentialErrorSVDNFramesWeighted(solution_r_gaussian,F_R);
        residualSynth_gaussian(count,j)=computerEssentialErrorSVDNFramesWeighted(solution_s_gaussian,F_S);

        disp(['j is ' num2str(j) ' and i is ' num2str(i)]);
    end

end