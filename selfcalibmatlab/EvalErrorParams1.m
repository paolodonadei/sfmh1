function [svScore, detScore, EssScore, EssScoreIA ] = EvalErrorParams1(F,f1,f2,x1,y1,x2,y2 )
svScore=0;
detScore=0;
EssScore=0;
EssScoreIA=0;



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
G=G;


detScore=det(G);

S = svd(G);



svScore =abs(S(1,1)-S(2,1))+abs(S(3,1));

RR=2*G*(G')*G- trace(G*(G'))*G;
%EssScoreIA=abs((2*(trace((G*(G')).^2)))-((trace(G*(G')))^2));
EssScoreIA=abs((2*(trace((G*(G'))*(G*(G')))))-((trace(G*(G')))^2));
RR
%EssScore=(2*((S(1,1)^4) +(S(2,1)^4)+(S(3,1)^4))) -(((S(1,1)^2) +(S(2,1)^2)+(S(3,1)^2))^2);
EssScore=sum(sum(abs(RR)));

% if (myerr>100)
%     myerr=100;
% end


%myerr
end

