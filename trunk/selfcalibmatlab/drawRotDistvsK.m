function  drawRotDistvsK(F,X,w,h)

Ke=eye(3);
Ke(1,3)=w/2;
Ke(2,3)=h/2;

counter=1;

for f1=200:1:2000
    clear K1 K2 E Pq P R q dij  djk dik
    K1=Ke;
    K1(1,1)=f1;
    K1(2,2)=f1;
    K2=K1;
    E= getEssentialMatrix(F,K1,K2);
    x = errorSingleF(K1,K2,F);
    svde(counter)=x;
    [Pq] = getCameraMatrix(E);
    [P] = getCorrectCameraMatrix(Pq, K1,K2, X);
    R = P(:,1:3);
    [q] = rotmat2quat(R);
    
    rm(counter)=q(1);
    
    mag=sqrt( (q(2)*q(2))+(q(3)*q(3))+(q(4)*q(4)));
    
    SX(1,counter)= q(2)/ mag ;
    SX(2,counter)= q(3)/mag  ;
    SX(3,counter)= q(4)/ mag ;
    %%%%%%%%%%%%%%%%%% distance
    
    dij = P(:,4);
    dij=dij/norm(dij);
      dX(1,counter)= dij(1) ;
    dX(2,counter)= dij(2) ;
    dX(3,counter)= dij(3) ;
    % normalizing
  
    
    
    t(counter)=f1;
    counter=counter+1;
end



plot(t,rm);
figure
plot(t,svde);
figure
scatter3(SX(1,:),SX(2,:),SX(3,:));

hold

[x1,y1,z1] = sphere(50);
X1 = [x1(:)];
Y1 = [y1(:)];
Z1 = [z1(:)];

scatter3(X1(:),Y1(:),Z1(:));

figure
scatter3(dX(1,:),dX(2,:),dX(3,:));


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