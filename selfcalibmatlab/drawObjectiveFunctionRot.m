function drawObjectiveFunctionRot(Ki,Kj,Kk,Fij,Fjk,Fik,Xij,Xjk,Xik,ks)

Ki=ks{1};
Kj=ks{2};
Kk=ks{3};

counter=1;
for f1=200:1:2000
  clear KIcan
  KIcan=Ki;
  KIcan(1,1)=f1;
  KIcan(2,2)=f1;
    

    t(counter)=f1;
  counter=counter+1;
end



end