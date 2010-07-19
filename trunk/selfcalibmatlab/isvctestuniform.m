clear all
clc
close all

numframes=3;
deviation1=40;
deviation2=80;
for k=1:100
    tic
    w=512;
    h=512;
    
[ F, ks ] = generateF( 0, 0, 1,(randn()*10)+deviation1,0,numframes,0,0 );

[fcl1_40, centerloc1] =nonlinsolveEssNfram(F,w,h);
[fclun1_40, centerlocun] =nonlinsolveEssNframuniform(F,w,h);
xp = PeterSturmSelfRobust( F,w,h);

errUN_40(k,1)=abs(fclun1_40(1,1)-ks{1}(1,1))+abs(fclun1_40(1,2)-ks{2}(1,1));
errNL_40(k,1)=abs(fcl1_40(1,1)-ks{1}(1,1))+abs(fcl1_40(1,2)-ks{2}(1,1));
errPS_40(k,1)=abs(xp(1,1)-ks{1}(1,1))+abs(xp(1,2)-ks{2}(1,1));

fclun1_40
fcl1_40
xp

display('errors 40');

errUN_40(k,1)
errNL_40(k,1)
errPS_40(k,1)


mean(errUN_40)
mean(errNL_40)
mean(errPS_40)

clear fcl1_40 fcl1_40Diff xp xh ks F

[ F, ks ] = generateF( 5, 3, 1,(randn()*10)+deviation2,0,numframes,0,0 );

[fcl1_90, centerloc1] =nonlinsolveEssNfram(F,w,h);
[fclun1_90, centerlocun2] =nonlinsolveEssNframuniform(F,w,h);
xp = PeterSturmSelfRobust( F,w,h);

errUN_90(k,1)=abs(fclun1_90(1,1)-ks{1}(1,1))+abs(fclun1_90(1,2)-ks{2}(1,1));
errNL_90(k,1)=abs(fcl1_90(1,1)-ks{1}(1,1))+abs(fcl1_90(1,2)-ks{2}(1,1));
errPS_90(k,1)=abs(xp(1,1)-ks{1}(1,1))+abs(xp(1,2)-ks{2}(1,1));

fclun1_90
fcl1_90
xp

display('errors 90');

errUN_90(k,1)
errNL_90(k,1)
errPS_90(k,1)

mean(errUN_90)
mean(errNL_90)
mean(errPS_90)

toc
k

end

title('results with 90 pixel deviation');
subplot(1,3,1), hist(errUN_90)
title('Uniform')
subplot(1,3,2), hist(errNL_90)
title('Proposed')
subplot(1,3,3), hist(errPS_90)
title('Kruppa')
figure
title('results with 40 pixel deviation');
subplot(1,3,1), hist(errUN_40)
title('Uniform')
subplot(1,3,2), hist(errNL_40)
title('Proposed')
subplot(1,3,3), hist(errPS_40)
title('Kruppa')
