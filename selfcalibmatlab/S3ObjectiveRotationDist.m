function x = S3ObjectiveRotationDist(p, data)

nowKs = convertLinetoMats(p,data{2,1}, data{3,1},data{4,1},data{5,1},data{6,1});


x=0;
count=0;

for i=1:data{4,1}
    for j=1:i
        if(i~=j)
            
            count=count+1;
             ersvd(count)=0;
            ersvd(count)=errorSingleF(nowKs{j},nowKs{i},data{1,1}{i,j});
            x=x+ ersvd(count);
        end
    end
end

count=0;
y=0;
for i=1:data{4,1}
    for j=1:data{4,1}
        for k=1:data{4,1}
            if(i~=j && j~=k && i~=k)
                
                count=count+1;
                erdist(count)=0;

                [ erdist(count), errot(count)]=errorthreeFdist(nowKs{i},nowKs{j},nowKs{k},data{1,1}{i,j},data{1,1}{j,k},data{1,1}{i,k},reshape(data{8,1}{i,j}(:,1),3,2),reshape(data{8,1}{j,k}(:,1),3,2),reshape(data{8,1}{i,k}(:,1),3,2));
                y=y+ erdist(count);
            end
        end
    end
end




[m,n]=size(p);
x=(ones(m,n)*x*1000000.0)/count;

x;
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

[KOij, Rij, tij] = vgg_KR_from_P(Pij);
[KOjk, Rjk, tjk] = vgg_KR_from_P(Pjk);
[KOik, Rik, tik] = vgg_KR_from_P(Pik);

[qij] = rotmat2quat(Rij);
[qjk] = rotmat2quat(Rjk);
[qik] = rotmat2quat(Rik);

x=0;
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