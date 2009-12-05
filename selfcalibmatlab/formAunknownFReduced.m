function [ A,b ] = formAunknownFReduced( P )


[m n ]=size(P);
numFrames=1;
numRowsperFrame=4; %this is how many equatiosn we get per frame, or how many linear constraints per picture

A=zeros(numFrames*numRowsperFrame,5);
b=zeros(numRowsperFrame*numFrames,1);


n=P{1,2};

%now augmenting the rows with zeros, this is necessary because we have to
%pass a matrix whose number of rows is bigger or equal to the columns to
%svd, so we have to add enough rows of zero to make the matrix square


A(1,1) = n(1,2) ^ 2 - n(2,1) ^ 2 + n(1,1) ^ 2 - n(2,2) ^ 2;
A(1,2) = 2 * n(1,1) * n(1,4) - 2 * n(2,1) * n(2,4);
A(1,3) = -2 * n(2,2) * n(2,4) + 2 * n(1,2) * n(1,4);
A(1,4) = 2 * n(1,3) * n(1,4) - 2 * n(2,3) * n(2,4);
A(1,5) = n(1,4) ^ 2 - n(2,4) ^ 2;
A(2,1) = n(3,2) * n(1,2) + n(3,1) * n(1,1);
A(2,2) = n(3,1) * n(1,4) + n(3,4) * n(1,1);
A(2,3) = n(3,4) * n(1,2) + n(3,2) * n(1,4);
A(2,4) = n(3,4) * n(1,3) + n(3,3) * n(1,4);
A(2,5) = n(3,4) * n(1,4);
A(3,1) = n(3,2) * n(2,2) + n(3,1) * n(2,1);
A(3,2) = n(3,1) * n(2,4) + n(3,4) * n(2,1);
A(3,3) = n(3,4) * n(2,2) + n(3,2) * n(2,4);
A(3,4) = n(3,4) * n(2,3) + n(3,3) * n(2,4);
A(3,5) = n(3,4) * n(2,4);
A(4,1) = n(2,2) * n(1,2) + n(2,1) * n(1,1);
A(4,2) = n(2,1) * n(1,4) + n(2,4) * n(1,1);
A(4,3) = n(2,4) * n(1,2) + n(2,2) * n(1,4);
A(4,4) = n(2,4) * n(1,3) + n(2,3) * n(1,4);
A(4,5) = n(2,4) * n(1,4);


b(1,1) = n(2,3) ^ 2 - n(1,3) ^ 2;
b(2,1) = -n(3,3) * n(1,3);
b(3,1) = -n(3,3) * n(2,3);
b(4,1) = -n(2,3) * n(1,3);

while(size(A,1)<size(A,2))
    
    A(end+1,:)=zeros(1,size(A,2));
    b(end+1,:)=zeros(1,1);
    
end





end