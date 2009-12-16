function [ A ] = uselessEqns( M )
[m, n]=size(M);

originalrank=rank(M);

for i=1:m
    Test=zeros(m-1,n);
    
    counter=1;
    for j=1:m
        if(j~=i)
            Test(counter,:)=M(j,:);
            counter=counter+1;
        end
    end
    
    
    rtest=rank(Test);
    if(rtest==originalrank)
        disp(['equation on line ' num2str(i) ' is useless']);
    end
    
    
    
    
end
A=M;


end

