function skew = skewsymmetrify3X3(RHS)

skew=zeros(3,3);

skew(0+1,1+1)= -RHS(2+1,0+1);
skew(0+1,2+1)= RHS(1+1,0+1);
skew(1+1,0+1)= RHS(2+1,0+1);
skew(1+1,2+1)= -RHS(0+1,0+1);
skew(2+1,0+1)= -RHS(1+1,0+1);
skew(2+1,1+1)= RHS(0+1,0+1);



end
