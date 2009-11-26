function [ A,b ] = formAunknownUX( P )


[m n ]=size(P);
numFrames=n;
numRowsperFrame=4; %this is how many equatiosn we get per frame, or how many linear constraints per picture

A=zeros(numFrames*numRowsperFrame,10);
b=zeros(numRowsperFrame*numFrames,1);

for i=1:numFrames
    
    b(((i-1)*numRowsperFrame)+1,1)=1;
    b(((i-1)*numRowsperFrame)+2,1)=1;
    
    m=   P{1,i};% to make it simpler we assign it
    
    
    A(((i-1)*numRowsperFrame)+1,1) = m(1,1) ^ 2;
    A(((i-1)*numRowsperFrame)+1,2) = 2 * m(1,1) * m(1,2);
    A(((i-1)*numRowsperFrame)+1,3) = 2 * m(1,1) * m(1,3);
    A(((i-1)*numRowsperFrame)+1,4) = 2 * m(1,1) * m(1,4);
    A(((i-1)*numRowsperFrame)+1,5) = m(1,2) ^ 2;
    A(((i-1)*numRowsperFrame)+1,6) = 2 * m(1,2) * m(1,3);
    A(((i-1)*numRowsperFrame)+1,7) = 2 * m(1,2) * m(1,4);
    A(((i-1)*numRowsperFrame)+1,8) = m(1,3) ^ 2;
    A(((i-1)*numRowsperFrame)+1,9) = 2 * m(1,3) * m(1,4);
    A(((i-1)*numRowsperFrame)+1,10) = m(1,4) ^ 2;
    
    A(((i-1)*numRowsperFrame)+2,1) = m(2,1) ^ 2;
    A(((i-1)*numRowsperFrame)+2,2) = 2 * m(2,1) * m(2,2);
    A(((i-1)*numRowsperFrame)+2,3) = 2 * m(2,1) * m(2,3);
    A(((i-1)*numRowsperFrame)+2,4) = 2 * m(2,1) * m(2,4);
    A(((i-1)*numRowsperFrame)+2,5) = m(2,2) ^ 2;
    A(((i-1)*numRowsperFrame)+2,6) = 2 * m(2,2) * m(2,3);
    A(((i-1)*numRowsperFrame)+2,7) = 2 * m(2,2) * m(2,4);
    A(((i-1)*numRowsperFrame)+2,8) = m(2,3) ^ 2;
    A(((i-1)*numRowsperFrame)+2,9) = 2 * m(2,3) * m(2,4);
    A(((i-1)*numRowsperFrame)+2,10) = m(2,4) ^ 2;
    
    A(((i-1)*numRowsperFrame)+3,1) = 2 * m(2,1) * m(1,1);
    A(((i-1)*numRowsperFrame)+3,2) = 2 * m(2,2) * m(1,1) + 2 * m(2,1) * m(1,2);
    A(((i-1)*numRowsperFrame)+3,3) = 2 * m(2,1) * m(1,3) + 2 * m(2,3) * m(1,1);
    A(((i-1)*numRowsperFrame)+3,4) = 2 * m(2,1) * m(1,4) + 2 * m(2,4) * m(1,1);
    A(((i-1)*numRowsperFrame)+3,5) = 2 * m(2,2) * m(1,2);
    A(((i-1)*numRowsperFrame)+3,6) = 2 * m(2,3) * m(1,2) + 2 * m(2,2) * m(1,3);
    A(((i-1)*numRowsperFrame)+3,7) = 2 * m(2,4) * m(1,2) + 2 * m(2,2) * m(1,4);
    A(((i-1)*numRowsperFrame)+3,8) = 2 * m(2,3) * m(1,3);
    A(((i-1)*numRowsperFrame)+3,9) = 2 * m(2,4) * m(1,3) + 2 * m(2,3) * m(1,4);
    A(((i-1)*numRowsperFrame)+3,10) = 2 * m(2,4) * m(1,4);
    
    A(((i-1)*numRowsperFrame)+4,1) = 2 * m(3,1) * m(2,1);
    A(((i-1)*numRowsperFrame)+4,2) = 2 * m(3,2) * m(2,1) + 2 * m(3,1) * m(2,2);
    A(((i-1)*numRowsperFrame)+4,3) = 2 * m(3,1) * m(2,3) + 2 * m(3,3) * m(2,1);
    A(((i-1)*numRowsperFrame)+4,4) = 2 * m(3,1) * m(2,4) + 2 * m(3,4) * m(2,1);
    A(((i-1)*numRowsperFrame)+4,5) = 2 * m(3,2) * m(2,2);
    A(((i-1)*numRowsperFrame)+4,6) = 2 * m(3,3) * m(2,2) + 2 * m(3,2) * m(2,3);
    A(((i-1)*numRowsperFrame)+4,7) = 2 * m(3,4) * m(2,2) + 2 * m(3,2) * m(2,4);
    A(((i-1)*numRowsperFrame)+4,8) = 2 * m(3,3) * m(2,3);
    A(((i-1)*numRowsperFrame)+4,9) = 2 * m(3,4) * m(2,3) + 2 * m(3,3) * m(2,4);
    A(((i-1)*numRowsperFrame)+4,10) = 2 * m(3,4) * m(2,4);
    
end

%now augmenting the rows with zeros, this is necessary because we have to
%pass a matrix whose number of rows is bigger or equal to the columns to
%svd, so we have to add enough rows of zero to make the matrix square
while(size(A,1)<size(A,2))
    
    A(end+1,:)=zeros(1,size(A,2));
    b(end+1,:)=zeros(1,1);
    
end





end