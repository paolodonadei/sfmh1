function [F] =PsTOFs(P)
%convert a set of projection matrices to a set of F matrices

[m,n]=size(P);

number=n;

if(number==1)
    disp(['error']);
    F=zeros(3);
elseif(number==2)
    F = vgg_F_from_P(P{1,1}, P{1,2});
else
    
    count=0;
    for i=1:number
        for j=1:i
            if(i~=j)
                count=count+1;
                F{count}=vgg_F_from_P(P{1,i},P{1,j});
            end
        end
    end
    
    
end