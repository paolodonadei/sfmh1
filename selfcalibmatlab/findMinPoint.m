function [ val, xindex, yindex,zindex ] = findMinPoint( M )
%find min of a matrix and its index

[z m n]=size(M);

minpt=M(1,1,1);
xindex=0;
yindex=0;
zindex=0;

for k=1:z
    for i=1:m
        for j=1:n
            if(M(k,i,j)<minpt)
                minpt=M(k,i,j);
                xindex=i;
                yindex=j;
                zindex=k;
            end
        end
    end
end
val=minpt;


