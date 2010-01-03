for k=1:200
    w=512;
    h=512;
    
[ F, ks ] = generateF( 0, 0, 1,(randn()*10)+40,0 );
xp = PeterSturmSelf( F,w,h);
[fcl1_40, centerloc1] = nonlinsolveEsstwofram(F,w,h);
[fcl2_40, centerloc1] = nonlinsolveEsstwofram2(F,w,h);
[fcl3_40, centerloc1] = nonlinsolveEsstwofram3(F,w,h);
[fcl4_40, centerloc1] = nonlinsolveEsstwofram4(F,w,h);
errPS_40(k,1)=abs(xp(1,1)-ks{1}(1,1));
errNL1_40(k,1)=abs(fcl1(1,1)-ks{1}(1,1));
errNL2_40(k,1)=abs(fcl2(1,1)-ks{1}(1,1));
errNL3_40(k,1)=abs(fcl3(1,1)-ks{1}(1,1));
errNL4_40(k,1)=abs(fcl4(1,1)-ks{1}(1,1));
errPS_40(k,1)
errNL1_40(k,1)
errNL2_40(k,1)
errNL3_40(k,1)
errNL4_40(k,1)

[ F, ks ] = generateF( 0, 0, 1,(randn()*10)+90,0 );
xp = PeterSturmSelf( F,w,h);
[fcl1_90, centerloc1] = nonlinsolveEsstwofram(F,w,h);
[fcl2_90, centerloc1] = nonlinsolveEsstwofram2(F,w,h);
[fcl3_90, centerloc1] = nonlinsolveEsstwofram3(F,w,h);
[fcl4_90, centerloc1] = nonlinsolveEsstwofram4(F,w,h);
errPS_90(k,1)=abs(xp(1,1)-ks{1}(1,1));
errNL1_90(k,1)=abs(fcl1(1,1)-ks{1}(1,1));
errNL2_90(k,1)=abs(fcl2(1,1)-ks{1}(1,1));
errNL3_90(k,1)=abs(fcl3(1,1)-ks{1}(1,1));
errNL4_90(k,1)=abs(fcl4(1,1)-ks{1}(1,1));
errPS_90(k,1)
errNL1_90(k,1)
errNL2_90(k,1)
errNL3_90(k,1)
errNL4_90(k,1)




k

end