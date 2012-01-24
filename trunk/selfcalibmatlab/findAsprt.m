function [A] = findAsprt(epsilon,omega)


Hgin=epsilon;
Hgout = 1-epsilon;
Hbin=omega;
Hbout=1-omega;
C=(Hbout*log(Hbout/Hgout))+(Hbin*log(Hbin/Hgin));
pg = epsilon^7;
tm=200;
ms=2.38;
K1= tm/pg;

K2= ms / (pg*C);


A=(K1/K2 ) +1;

for i=1:17
A=(K1/K2)+1+log(A);
end


end