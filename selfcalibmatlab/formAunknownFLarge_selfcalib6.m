function [ A,b ] = formAunknownFLarge_selfcalib6( P )


[m n ]=size(P);
numFrames=n;
numRowsperFrame=6; %this is how many equatiosn we get per frame, or how many linear constraints per picture

A=zeros(numFrames*numRowsperFrame,10);
b=zeros(numRowsperFrame*numFrames,1);




for i=1:numFrames
    m=   P{1,i};% to make it simpler we assign it
    j=1;
    
    A(((i-1)*numRowsperFrame)+j,1) = (m(1,1) ^ 2) - (m(2,1) ^ 2);
    A(((i-1)*numRowsperFrame)+j,2) = -2 * m(2,1) * m(2,2) + 2 * m(1,1) * m(1,2);
    A(((i-1)*numRowsperFrame)+j,3) = 2 * m(1,1) * m(1,3) - 2 * m(2,1) * m(2,3);
    A(((i-1)*numRowsperFrame)+j,4) = 2 * m(1,1) * m(1,4) - 2 * m(2,4) * m(2,1);
    A(((i-1)*numRowsperFrame)+j,5) = m(1,2) ^ 2 - m(2,2) ^ 2;
    A(((i-1)*numRowsperFrame)+j,6) = 2 * m(1,2) * m(1,3) - 2 * m(2,3) * m(2,2);
    A(((i-1)*numRowsperFrame)+j,7) = 2 * m(1,4) * m(1,2) - 2 * m(2,4) * m(2,2);
    A(((i-1)*numRowsperFrame)+j,8) = -m(2,3) ^ 2 + m(1,3) ^ 2;
    A(((i-1)*numRowsperFrame)+j,9) = -2 * m(2,3) * m(2,4) + 2 * m(1,3) * m(1,4);
    A(((i-1)*numRowsperFrame)+j,10) = m(1,4) ^ 2 - m(2,4) ^ 2;
    
    j=j+1;
    
    A(((i-1)*numRowsperFrame)+j,1) = m(2,1) ^ 2 - m(3,1) ^ 2;
    A(((i-1)*numRowsperFrame)+j,2) = -2 * m(3,1) * m(3,2) + 2 * m(2,1) * m(2,2);
    A(((i-1)*numRowsperFrame)+j,3) = 2 * m(2,1) * m(2,3) - 2 * m(3,1) * m(3,3);
    A(((i-1)*numRowsperFrame)+j,4) = 2 * m(2,4) * m(2,1) - 2 * m(3,4) * m(3,1);
    A(((i-1)*numRowsperFrame)+j,5) = m(2,2) ^ 2 - m(3,2) ^ 2;
    A(((i-1)*numRowsperFrame)+j,6) = 2 * m(2,3) * m(2,2) - 2 * m(3,3) * m(3,2);
    A(((i-1)*numRowsperFrame)+j,7) = 2 * m(2,4) * m(2,2) - 2 * m(3,4) * m(3,2);
    A(((i-1)*numRowsperFrame)+j,8) = -m(3,3) ^ 2 + m(2,3) ^ 2;
    A(((i-1)*numRowsperFrame)+j,9) = -2 * m(3,3) * m(3,4) + 2 * m(2,3) * m(2,4);
    A(((i-1)*numRowsperFrame)+j,10) = m(2,4) ^ 2 - m(3,4) ^ 2;
    
    j=j+1;
    
    A(((i-1)*numRowsperFrame)+j,1) = m(3,1) ^ 2 - m(1,1) ^ 2;
    A(((i-1)*numRowsperFrame)+j,2) = -2 * m(1,1) * m(1,2) + 2 * m(3,1) * m(3,2);
    A(((i-1)*numRowsperFrame)+j,3) = 2 * m(3,1) * m(3,3) - 2 * m(1,1) * m(1,3);
    A(((i-1)*numRowsperFrame)+j,4) = 2 * m(3,4) * m(3,1) - 2 * m(1,1) * m(1,4);
    A(((i-1)*numRowsperFrame)+j,5) = m(3,2) ^ 2 - m(1,2) ^ 2;
    A(((i-1)*numRowsperFrame)+j,6) = 2 * m(3,3) * m(3,2) - 2 * m(1,2) * m(1,3);
    A(((i-1)*numRowsperFrame)+j,7) = 2 * m(3,4) * m(3,2) - 2 * m(1,4) * m(1,2);
    A(((i-1)*numRowsperFrame)+j,8) = -m(1,3) ^ 2 + m(3,3) ^ 2;
    A(((i-1)*numRowsperFrame)+j,9) = -2 * m(1,3) * m(1,4) + 2 * m(3,3) * m(3,4);
    A(((i-1)*numRowsperFrame)+j,10) = m(3,4) ^ 2 - m(1,4) ^ 2;
    
    j=j+1;
    
    A(((i-1)*numRowsperFrame)+j,1) = m(2,1) * m(1,1);
    A(((i-1)*numRowsperFrame)+j,2) = m(2,2) * m(1,1) + m(2,1) * m(1,2);
    A(((i-1)*numRowsperFrame)+j,3) = m(2,1) * m(1,3) + m(2,3) * m(1,1);
    A(((i-1)*numRowsperFrame)+j,4) = m(2,1) * m(1,4) + m(2,4) * m(1,1);
    A(((i-1)*numRowsperFrame)+j,5) = m(2,2) * m(1,2);
    A(((i-1)*numRowsperFrame)+j,6) = m(2,3) * m(1,2) + m(2,2) * m(1,3);
    A(((i-1)*numRowsperFrame)+j,7) = m(2,4) * m(1,2) + m(2,2) * m(1,4);
    A(((i-1)*numRowsperFrame)+j,8) = m(2,3) * m(1,3);
    A(((i-1)*numRowsperFrame)+j,9) = m(2,4) * m(1,3) + m(2,3) * m(1,4);
    A(((i-1)*numRowsperFrame)+j,10) = m(2,4) * m(1,4);
    
    j=j+1;
    
    A(((i-1)*numRowsperFrame)+j,1) = m(3,1) * m(2,1);
    A(((i-1)*numRowsperFrame)+j,2) = m(3,2) * m(2,1) + m(3,1) * m(2,2);
    A(((i-1)*numRowsperFrame)+j,3) = m(3,1) * m(2,3) + m(3,3) * m(2,1);
    A(((i-1)*numRowsperFrame)+j,4) = m(3,1) * m(2,4) + m(3,4) * m(2,1);
    A(((i-1)*numRowsperFrame)+j,5) = m(3,2) * m(2,2);
    A(((i-1)*numRowsperFrame)+j,6) = m(3,3) * m(2,2) + m(3,2) * m(2,3);
    A(((i-1)*numRowsperFrame)+j,7) = m(3,4) * m(2,2) + m(3,2) * m(2,4);
    A(((i-1)*numRowsperFrame)+j,8) = m(3,3) * m(2,3);
    A(((i-1)*numRowsperFrame)+j,9) = m(3,4) * m(2,3) + m(3,3) * m(2,4);
    A(((i-1)*numRowsperFrame)+j,10) = m(3,4) * m(2,4);
    
    j=j+1;
    
    A(((i-1)*numRowsperFrame)+j,1) = m(3,1) * m(1,1);
    A(((i-1)*numRowsperFrame)+j,2) = m(3,2) * m(1,1) + m(3,1) * m(1,2);
    A(((i-1)*numRowsperFrame)+j,3) = m(3,1) * m(1,3) + m(3,3) * m(1,1);
    A(((i-1)*numRowsperFrame)+j,4) = m(3,1) * m(1,4) + m(3,4) * m(1,1);
    A(((i-1)*numRowsperFrame)+j,5) = m(3,2) * m(1,2);
    A(((i-1)*numRowsperFrame)+j,6) = m(3,3) * m(1,2) + m(3,2) * m(1,3);
    A(((i-1)*numRowsperFrame)+j,7) = m(3,4) * m(1,2) + m(3,2) * m(1,4);
    A(((i-1)*numRowsperFrame)+j,8) = m(3,3) * m(1,3);
    A(((i-1)*numRowsperFrame)+j,9) = m(3,4) * m(1,3) + m(3,3) * m(1,4);
    A(((i-1)*numRowsperFrame)+j,10) = m(3,4) * m(1,4);
    
    
    
    
    
    
end

%now augmenting the rows with zeros, this is necessary because we have to
%pass a matrix whose number of rows is bigger or equal to the columns to
%svd, so we have to add enough rows of zero to make the matrix square
while(size(A,1)<size(A,2))
    
    A(end+j,:)=zeros(1,size(A,2));
    
end




end