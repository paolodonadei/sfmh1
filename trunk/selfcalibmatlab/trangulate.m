function [X]=trangulate(P1,P2,x1,x2)


%normalize
x1=x1/x1(3,1);
x2=x2/x2(3,1);

%form A matrix
A=[x1(1,1)*P1(3,:) - P1(1,:) ;
   x1(2,1)*P1(3,:) - P1(2,:) ;  
   x2(1,1)*P2(3,:) - P2(1,:) ;
   x2(2,1)*P2(3,:) - P2(2,:) ];

[U,S,V] = svd(A);
X=V(:,4);

   X(1,1)=X(1,1)/X(4,1);
    X(2,1)=X(2,1)/X(4,1);
    X(3,1)=X(3,1)/X(4,1);
%     
%     
%     fprintf('_______________________\n');
%       fprintf('point [%f  %f]\t',x1(1,1),x1(2,1));
%         P1   
%               fprintf('point [%f  %f]\t',x2(1,1),x2(2,1));
%         P2   
%         fprintf('mapped point to [%f %f %f]\n'  , X(1,1),X(2,1), X(3,1));
end