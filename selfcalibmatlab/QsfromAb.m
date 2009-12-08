function [Q]=QsfromAb(A,b)
%this is for the rank deficient version that works with 2 frames


%so this function has to check b, if b is all zeros then we do the
%simplified version which is just the last two vectors of V, otherwise we
%gonna folow the recipe in the multiple view geomtry book's appendic and
%get the two solutions

Q1=zeros(4,4);
Q2=zeros(4,4);

biszeros=1;

[m n]=size(b);


for i=1:m
    if(b(i,1)>0)
        biszeros=0;
        break;
    end
end


[U,S,V] = svd(A);
r=rank(A);


if(r<10)
    if(biszeros==1)
        V(:,10);
        V(:,9);
        
        Q1=myFormatvectoQ(V(:,10));
        Q2=myFormatvectoQ(V(:,9));
        
    else
        y=zeros(10,1);
        bp=(U')*b;
        
        
        for i=1:r
            y(i,1)=bp(i,1)/S(i,i);
        end
        X=V*y;
        X
        V(:,10)
        Q1=myFormatvectoQ(X);
        Q2=myFormatvectoQ(V(:,10)); %im not sure if this is right, i took this from page 590 of multiple view book and also from a pdf on the net called sabes.pdf that contains tutorial info on svd
        
    end
    Q={Q1 Q2};
    
else
    if(biszeros==1)
        
        
        Q1=myFormatvectoQ(V(:,10));

        
    else
        y=zeros(10,1);
        bp=(U')*b;
        
        
        for i=1:r
            y(i,1)=bp(i,1)/S(i,i);
        end
        X=V*y
        Q1=myFormatvectoQ(X);
      
    end
         Q={Q1};
    
    
    
    
end





%disp([ 'last svd is ' num2str(S(10,10)) ' and the condition number is (it should be near 1) ' num2str(cond(A)) ' and the rank is ' num2str(rank(A))] );



end
