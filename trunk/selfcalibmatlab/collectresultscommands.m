for k=1:100
    tic
    w=512;
    h=512;
    
[ F, ks ] = generateF( 0, 0, 1,(randn()*10)+40,0 );
xp = PeterSturmSelf( F,w,h);
[fcl1_40, centerloc1] = nonlinsolveEsstwofram(F,w,h);
[fcl2_40, centerloc1] = nonlinsolveEsstwofram2(F,w,h);
[fcl3_40, centerloc1] = nonlinsolveEsstwofram3(F,w,h);
[fcl4_40, centerloc1] = nonlinsolveEsstwofram4(F,w,h);
errPS_40(k,1)=abs(xp(1,1)-ks{1}(1,1));
errNL1_40(k,1)=abs(fcl1_40(1,1)-ks{1}(1,1));
errNL2_40(k,1)=abs(fcl2_40(1,1)-ks{1}(1,1));
errNL3_40(k,1)=abs(fcl3_40(1,1)-ks{1}(1,1));
errNL4_40(k,1)=abs(fcl4_40(1,1)-ks{1}(1,1));
errPS_40(k,1)
errNL1_40(k,1)
errNL2_40(k,1)
errNL3_40(k,1)
errNL4_40(k,1)

[ F9, ks9 ] = generateF( 0, 0, 1,(randn()*10)+90,0 );
xp = PeterSturmSelf( F9,w,h);
[fcl1_90, centerloc1] = nonlinsolveEsstwofram(F9,w,h);
[fcl2_90, centerloc1] = nonlinsolveEsstwofram2(F9,w,h);
[fcl3_90, centerloc1] = nonlinsolveEsstwofram3(F9,w,h);
[fcl4_90, centerloc1] = nonlinsolveEsstwofram4(F9,w,h);
errPS_90(k,1)=abs(xp(1,1)-ks9{1}(1,1));
errNL1_90(k,1)=abs(fcl1_90(1,1)-ks9{1}(1,1));
errNL2_90(k,1)=abs(fcl2_90(1,1)-ks9{1}(1,1));
errNL3_90(k,1)=abs(fcl3_90(1,1)-ks9{1}(1,1));
errNL4_90(k,1)=abs(fcl4_90(1,1)-ks9{1}(1,1));
errPS_90(k,1)
errNL1_90(k,1)
errNL2_90(k,1)
errNL3_90(k,1)
errNL4_90(k,1)



toc
k

end


%%%%%%%%%%%%%%%% for two frames

for k=1:100
    tic
    w=512;
    h=512;
    
[ F, ks ] = generateF( (randn()*10)+40, 0, 1,(randn()*10)+40,0 );
xp = PeterSturmSelf( F,w,h);
xh = HartleySelf( F,w,h);
[fcl1_40, centerloc1] = nonlinsolveEsstwofram(F,w,h);
[fcl1_40Diff, centerloc1] = nonlinsolveEsstwoframDiffFrames(F,w,h);

errPS_40(k,1)=abs(xp(1,1)-ks{1}(1,1))+abs(xp(1,2)-ks{2}(1,1));
errRH_40(k,1)=abs(xh(1,1)-ks{1}(1,1))+abs(xh(1,2)-ks{2}(1,1));;
errNL_40(k,1)=abs(fcl1_40(1,1)-ks{1}(1,1))+abs(fcl1_40(1,2)-ks{2}(1,1));;
errNLDIFF_40(k,1)=abs(fcl1_40Diff(1,1)-ks{1}(1,1))+abs(fcl1_40Diff(1,2)-ks{2}(1,1));;

xp
xh
fcl1_40
fcl1_40Diff

display('errors 40');

errPS_40(k,1)
errRH_40(k,1)
errNL_40(k,1)
errNLDIFF_40(k,1)

clear fcl1_40 fcl1_40Diff xp xh ks F

[ F9, ks9 ] = generateF( (randn()*10)+90, 0, 1,(randn()*10)+40,0 );

xp9 = PeterSturmSelf( F9,w,h);
xh9 = HartleySelf( F9,w,h);
[fcl1_90, centerloc1] = nonlinsolveEsstwofram(F9,w,h);
[fcl1_90Diff, centerloc1] = nonlinsolveEsstwoframDiffFrames(F9,w,h);

errPS_90(k,1)=abs(xp9(1,1)-ks9{1}(1,1))+abs(xp9(1,2)-ks9{2}(1,1));
errRH_90(k,1)=abs(xh9(1,1)-ks9{1}(1,1))+abs(xh9(1,2)-ks9{2}(1,1));
errNL_90(k,1)=abs(fcl1_90(1,1)-ks9{1}(1,1))+abs(fcl1_90(1,2)-ks9{2}(1,1));
errNLDIFF_90(k,1)=abs(fcl1_90Diff(1,1)-ks9{1}(1,1))+abs(fcl1_90Diff(1,2)-ks9{2}(1,1));


xp9
xh9
fcl1_90
fcl1_90Diff

display('errors 90');

errPS_90(k,1)
errRH_90(k,1)
errNL_90(k,1)
errNLDIFF_90(k,1)


toc
k

end