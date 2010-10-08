
function x = S3Objective(p, data)

nowKs = convertLinetoMats(p,data{2,1}, data{3,1},data{4,1},data{5,1},data{6,1});


x=0;
for i=1:data{4,1}
    for j=i:data{4,1}
        if(i~=j)
            x=x+ errorSingleF(nowKs{j},nowKs{i},data{1,1}{i,j});
        end
    end
end

[m,n]=size(p);
x=ones(m,n)*x;


end

function x = errorSingleF(K1,K2,F)


G=(K1')*F*K2;

try
    S = svd(G);
catch
    S= [ 100 ;  10 ;  5];
end

if( S(2,1)>eps)
    x=(((S(1,1)-S(2,1))/S(2,1)))^2; % should not be squared but all my thresholds are based on this being squared
    
else
    
    x=100;
end


end