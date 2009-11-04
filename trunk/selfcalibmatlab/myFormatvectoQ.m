function [ output_args ] = myFormatvectoQ( longV )

output_args=zeros(4,4);
[m n]=size(longV);

if(m~=10 || n~=1)
   message=['the size of the vector inside myformattingQ is wrong'] 
end

z=1;
for i=1:4
    for j=i:4
      %  M=[' i is ' num2str(i) ' j is ' num2str(j) ' z is ' num2str(z) ' value is ' num2str(longV(z))]
    output_args(i,j)=longV(z);
    output_args(j,i)=longV(z);
    z=z+1;
    end
end

