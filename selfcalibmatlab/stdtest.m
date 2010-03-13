clear all
clc
close all
count=0;



for j=1:1000
    [ F_Sg, ks_S ] = generateF( 0, 0,1.01,40,1,5,0.01,5  );


    solution_s=[ks_S{1,1}(1,1) ks_S{1,1}(1,3) ks_S{1,1}(2,3) ks_S{1,1}(2,2)];


    residualSynthgood(j)=computerEssentialErrorSVDNFramesWeighted(solution_s,F_Sg(1,6:10));
    residualSynthbad(j)=computerEssentialErrorSVDNFramesWeighted(solution_s,F_Sg(1,1:5));
    residualSynthall(j)=computerEssentialErrorSVDNFramesWeighted(solution_s,F_Sg);

    disp(['j is ' num2str(j) ]);
end

tbad=prctile(residualSynthbad,99);
tgood=prctile(residualSynthgood,99);


for j=1:1000
   if( residualSynthbad(1,j) >tbad)
       residualSynthbad(1,j)=median(residualSynthbad);
   end
   if( residualSynthgood(1,j) >tgood)
      residualSynthgood(1,j) =median(residualSynthgood);
   end    
end



subplot(1,2,1)
hist(residualSynthbad,100);
title('corrupted F matrices')
subplot(1,2,2)
hist(residualSynthgood,100);
title('clean F matrices')





  saveas(gcf,['distributions.jpg']);
    saveas(gcf,['distributions.fig']);
    saveas(gcf,['distributions.eps'],'epsc');