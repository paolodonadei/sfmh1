function [] =runSelfCalibFindErrors( F , ks, funcName,w,h)

[answerf, loca]=funcName(F,w,h)

errf1=abs(answerf(1,1)-ks{1}(1,1));
errx1=abs(loca(1,1)-ks{1}(1,3));
erry1=abs(loca(1,2)-ks{1}(2,3));
disp(['error in f1 was ' num2str(errf1) ' and error in x1 was ' num2str(errx1) ' and error in y1 was ' num2str(erry1)]);

[m, n]=size(loca);

if(n>3)
    
errf2=abs(answerf(1,2)-ks{2}(1,1));
errx2=abs(loca(1,3)-ks{2}(1,3));
erry2=abs(loca(1,4)-ks{2}(2,3));

disp(['error in f2 was ' num2str(errf2) ' and error in x2 was ' num2str(errx2) ' and error in y2 was ' num2str(erry2)]);

end

end
