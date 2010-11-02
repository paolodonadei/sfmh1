function [err] = threeFrameErrors(p, data)



nowKs = convertLinetoMats(p,data{2,1}, data{3,1},data{4,1},data{5,1},data{6,1});


x=0;
countsvd=0;

for i=1:data{4,1}
    for j=1:i
        if(i~=j)
            
            countsvd=countsvd+1;
             ersvd(countsvd)=0;
            ersvd(countsvd)=errorSingleF(nowKs{j},nowKs{i},data{1,1}{i,j});
            x=x+ ersvd(countsvd);
        end
    end
end

countrot=0;
y=0;
% for i=1:data{4,1}
%     for j=1:data{4,1}
%         for k=1:data{4,1}
%             if(i~=j && j~=k && i~=k)
%                 
%                 countrot=countrot+1;
%                 erdist(countrot)=0;
% 
%                 [ erdist(countrot), errot(countrot)]=errorthreeFdist(nowKs{i},nowKs{j},nowKs{k},data{1,1}{i,j},data{1,1}{j,k},data{1,1}{i,k},reshape(data{8,1}{i,j}(:,1),3,2),reshape(data{8,1}{j,k}(:,1),3,2),reshape(data{8,1}{i,k}(:,1),3,2));
%                 y=y+  errot(countrot);
%             end
%         end
%     end
% end










end