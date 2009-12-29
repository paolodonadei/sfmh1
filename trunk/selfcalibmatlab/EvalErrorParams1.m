function [myerr ] = EvalErrorParams1(F,f1,f2,x1,y1,x2,y2 )
myerr=0;
K_norm1=zeros(3,3);
K_norm2=zeros(3,3);

K_norm1(1,1)=  f1;
K_norm1(2,2)=  f1;
K_norm1(3,3)=  1;
K_norm1(1,3)= x1;
K_norm1(2,3)= y1;


K_norm2(1,1)= f2;
K_norm2(2,2)= f2;
K_norm2(3,3)=  1;
K_norm2(3,1)= x2;
K_norm2(3,2)= y2;




G=(K_norm2)*F*K_norm1;
%G=G/G(3,3);

%S = svd(G);



%myerr =abs(S(1,1)-S(2,1))+abs(S(3,1));

RR=2*G*(G')*G- trace(G*(G'))*G;
%RR
myerr=myerr+sum(sum(abs(RR)));

% if (myerr>100)
%     myerr=100;
% end


%myerr
end

