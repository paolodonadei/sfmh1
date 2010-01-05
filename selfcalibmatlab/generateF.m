function [ F, ks,ps ] = generateF( fdiff, skew, ar,centerdeviation,silent,number )


[ ps ,myks ] = generatePs( fdiff, skew, ar,centerdeviation, number,1,silent );

P1=ps{1,1};
P2=ps{1,2};


if(number==1)
    disp(['error']);
    F=zeros(3);
elseif(number==2)
    F = vgg_F_from_P(P1, P2);
else
    
    count=0;
    for i=1:number
        for j=1:i
            if(i~=j)
                count=count+1;
                F{count}=vgg_F_from_P(ps{1,i},ps{1,j});
            end
        end
    end
    
    
end




ks=myks;





end