function [ A,b ] = formAunknownF( P )


[m n ]=size(P);
numFrames=n;
numRowsperFrame=4; %this is how many equatiosn we get per frame, or how many linear constraints per picture

A=zeros(numFrames*numRowsperFrame,10);
b=zeros(numRowsperFrame*numFrames,1);

for i=1:numFrames
    m=   P{1,i};% to make it simpler we assign it


    A(1+((i-1)*numRowsperFrame), 1)  =  (Pow(m(1 , 1), 2.0) - Pow(m(2 , 1), 2.0));
    A(1+((i-1)*numRowsperFrame) , 2) =  (-2.0 * m(2 , 1) * m(2 , 2) + 2.0 * m(1 , 1) * m(1 , 2));
    A(1+((i-1)*numRowsperFrame), 3)  =  (2.0 * m(1 , 1) * m(1 , 3) - 2.0 * m(2 , 1) * m(2 , 3));
    A(1+((i-1)*numRowsperFrame) , 4) =  (2.0 * m(1 , 1) * m(1 , 4) - 2.0 * m(2 , 1) * m(2 , 4));
    A(1+((i-1)*numRowsperFrame) , 5) =  (Pow(m(1 , 2), 2.0) - Pow(m(2 , 2), 2.0));
    A(1+((i-1)*numRowsperFrame) , 6) =  (2.0 * m(1 , 2) * m(1 , 3) - 2.0 * m(2 , 2) * m(2 , 3));
    A(1+((i-1)*numRowsperFrame), 7)  =  (2.0 * m(1 , 4) * m(1 , 2) - 2.0 * m(2 , 4) * m(2 , 2));
    A(1+((i-1)*numRowsperFrame) , 8) =  (-Pow(m(2 , 3), 2.0) + Pow(m(1 , 3), 2.0));
    A(1+((i-1)*numRowsperFrame) , 9) =  (-2.0 * m(2 , 3) * m(2 , 4) + 2.0 * m(1 , 3) * m(1 , 4));
    A(1+((i-1)*numRowsperFrame) , 10)= (Pow(m(1 , 4), 2.0) - Pow(m(2 , 4), 2.0));

    A(2+((i-1)*numRowsperFrame) , 1) =  m(2 , 1) * m(1 , 1);
    A(2+((i-1)*numRowsperFrame), 2)  =  (m(2 , 2) * m(1 , 1) + m(2 , 1) * m(1 , 2));
    A(2+((i-1)*numRowsperFrame) , 3) =  (m(2 , 1) * m(1 , 3) + m(2 , 3) * m(1 , 1));
    A(2+((i-1)*numRowsperFrame), 4)  =  (m(2 , 1) * m(1 , 4) + m(2 , 4) * m(1 , 1));
    A(2+((i-1)*numRowsperFrame) , 5) =   m(2 , 2) * m(1 , 2);
    A(2+((i-1)*numRowsperFrame) , 6) =  (m(2 , 3) * m(1 , 2) + m(2 , 2) * m(1 , 3));
    A(2+((i-1)*numRowsperFrame) , 7) =  (m(2 , 4) * m(1 , 2) + m(2 , 2) * m(1 , 4));
    A(2+((i-1)*numRowsperFrame) , 8) =   m(2 , 3) * m(1 , 3);
    A(2+((i-1)*numRowsperFrame) , 9) =  (m(2 , 4) * m(1 , 3) + m(2 , 3) * m(1 , 4));
    A(2+((i-1)*numRowsperFrame), 10) =  m(2 , 4) * m(1 , 4);

    A(3+((i-1)*numRowsperFrame) , 1) =   m(3 , 1) * m(1 , 1);
    A(3+((i-1)*numRowsperFrame) , 2) =  (m(3 , 2) * m(1 , 1) + m(3 , 1) * m(1 , 2));
    A(3+((i-1)*numRowsperFrame), 3)  =  (m(3 , 1) * m(1 , 3) + m(3 , 3) * m(1 , 1));
    A(3+((i-1)*numRowsperFrame), 4)  =  (m(3 , 1) * m(1 , 4) + m(3 , 4) * m(1 , 1));
    A(3+((i-1)*numRowsperFrame) , 5) =   m(3 , 2) * m(1 , 2);
    A(3 +((i-1)*numRowsperFrame), 6) =  (m(3 , 3) * m(1 , 2) + m(3 , 2) * m(1 , 3));
    A(3+((i-1)*numRowsperFrame) , 7) =  (m(3 , 4) * m(1 , 2) + m(3 , 2) * m(1 , 4));
    A(3+((i-1)*numRowsperFrame) , 8) =   m(3 , 3) * m(1 , 3);
    A(3+((i-1)*numRowsperFrame), 9)  =  (m(3 , 4) * m(1 , 3) + m(3 , 3) * m(1 , 4));
    A(3+((i-1)*numRowsperFrame) , 10)=  m(3 , 4) * m(1 , 4);

    A(4+((i-1)*numRowsperFrame), 1)  =   m(3 , 1) * m(2 , 1);
    A(4+((i-1)*numRowsperFrame), 2)  =  (m(3 , 2) * m(2 , 1) + m(3 , 1) * m(2 , 2));
    A(4+((i-1)*numRowsperFrame) , 3) =  (m(3 , 1) * m(2 , 3) + m(3 , 3) * m(2 , 1));
    A(4+((i-1)*numRowsperFrame) , 4) =  (m(3 , 1) * m(2 , 4) + m(3 , 4) * m(2 , 1));
    A(4+((i-1)*numRowsperFrame) , 5) =  m(3 , 2) * m(2 , 2);
    A(4+((i-1)*numRowsperFrame) , 6) =  (m(3 , 3) * m(2 , 2) + m(3 , 2) * m(2 , 3));
    A(4+((i-1)*numRowsperFrame), 7)  =  (m(3 , 4) * m(2 , 2) + m(3 , 2) * m(2 , 4));
    A(4+((i-1)*numRowsperFrame), 8)  =  m(3 , 3) * m(2 , 3);
    A(4+((i-1)*numRowsperFrame) , 9) =  (m(3 , 4) * m(2 , 3) + m(3 , 3) * m(2 , 4));
    A(4+((i-1)*numRowsperFrame) , 10)=  m(3 , 4) * m(2 , 4);

end

%now augmenting the rows with zeros, this is necessary because we have to
%pass a matrix whose number of rows is bigger or equal to the columns to
%svd, so we have to add enough rows of zero to make the matrix square
while(size(A,1)<size(A,2))

    A(end+1,:)=zeros(1,size(A,2));

end

%multiplying the rows

for i=1:numFrames
    A(1+((i-1)*numRowsperFrame),:)=A(1+((i-1)*numRowsperFrame),:);
    A(2+((i-1)*numRowsperFrame),:)=A(2+((i-1)*numRowsperFrame),:)*(2);
    A(3+((i-1)*numRowsperFrame),:)=A(3+((i-1)*numRowsperFrame),:)*(2);
    A(4+((i-1)*numRowsperFrame),:)=A(4+((i-1)*numRowsperFrame),:)*(2);%changed this, i think paper is wrong
end



end