function [x] = S3ObjectiveRotationDist(p, data)

nowKs = convertLinetoMats(p,data{2,1}, data{3,1},data{4,1},data{5,1},data{6,1});

%%%%%%%%%%%%%%%%%%% SVD ERROR %%%%%%%%%%%%%%
x=0;
countsvd=0;

for i=1:data{4,1}
    for j=1:i
        if(i~=j)
            
            countsvd=countsvd+1;
             ersvd(countsvd)=0;
            ersvd(countsvd)=errorSingleF(nowKs{j},nowKs{i},data{1,1}{i,j});
            x=x+ ersvd(countsvd);
        end
    end
end
x=x/countsvd;
if(countsvd==0)
    display(['erorr , count svd is zero']);
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% DIST AND ROT ERROR


i=1;j=2;k=3;
[distEr, rotEr]=errorthreeFdist(nowKs{i},nowKs{j},nowKs{k},data{1,1}{i,j},data{1,1}{j,k},data{1,1}{i,k},reshape(data{8,1}{i,j}(:,1),3,2),reshape(data{8,1}{j,k}(:,1),3,2),reshape(data{8,1}{i,k}(:,1),3,2));



y=distEr;
y=rotEr;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% END OF DIST AND ROT ERROR,
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% NOW MIX THE ERRORS


A=0;
B=1-A;

finalError=(A*x)+(B*y);
disp(['svd error is ' num2str(x/countsvd) ' and rotation error is ' num2str(y) ' and final error is ' num2str(finalError*1000000.0) ]);




[m,n]=size(p);
z=(ones(m,n)*finalError*1);

x=z;
%x
end
function [x,q] = errorthreeFdist(Ki,Kj,Kk,Fij,Fjk,Fik,Xij,Xjk,Xik)

Eij = getEssentialMatrix(Fij,Ki,Kj);
Ejk = getEssentialMatrix(Fjk,Kj,Kk);
Eik = getEssentialMatrix(Fik,Ki,Kk);



[PijC] = getCameraMatrix(Eij);
[PjkC] = getCameraMatrix(Ejk);
[PikC] = getCameraMatrix(Eik);

[Pij] = getCorrectCameraMatrix(PijC, Ki,Kj, Xij);
[Pjk] = getCorrectCameraMatrix(PjkC, Kj,Kk, Xjk);
[Pik] = getCorrectCameraMatrix(PikC, Ki,Kk, Xik);


% when decomposing the projection marix the signs get weird, and since i
% did not multiply by K im just gonna copy the R part
Rij = Pij(:,1:3);
Rjk = Pjk(:,1:3);
Rik = Pik(:,1:3);

[qij] = rotmat2quat(Rij);
[qjk] = rotmat2quat(Rjk);
[qik] = rotmat2quat(Rik);

interRot=quatmultiply(qjk,qij );
conjRik=quatconj(qik);
QRes=  quatmultiply(conjRik,interRot );
%QRes2= slerp (quatconj(qik), quatmultiply(qjk,qij ), 1, 1);



q=abs(QRes(1,1));
%%%%%%%% now calculate the distance error

% dij = -inv(Rij)*Pij(:,4);
% djk = -inv(Rjk)*Pjk(:,4);
% dik = -inv(Rik)*Pik(:,4);
dij =Pij(:,4);
djk = Pjk(:,4);
dik = Pik(:,4);
b=[dij djk dik];
[ Ki(1,1) Kj(1,1) Kk(1,1)];;


b;;
% normalizing
[x] = findError3angle(Pij, Pjk, Pik);

x=abs(x);

end
function x = errorSingleF(K1,K2,F)


G=(K1')*F*K2;

try
    S = svd(G);
catch
    S= [ 100 ;  10 ;  5];
end

if( S(2,1)>eps)
    x=(((S(1,1)-S(2,1))/S(2,1))); % should not be squared but all my thresholds are based on this being squared
    
else
    
    x=100;
end


end