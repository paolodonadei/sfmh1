function [ err ] = calcMerror( M,P_in )
%calculate the error of the abs quadric

err=0;
M=M/norm(M,'fro');

for i=1:size(P_in,2)
    P=P_in{1,i}/norm(P_in{1,i},'fro');
    
    err1=P(1,:) * M * (P(1,:)') - P(2,:) * M * (P(2,:)') ;
    err2=P(1,:) * M * (P(1,:)') - P(3,:) * M * (P(3,:)') ;
    err3=P(2,:) * M * (P(2,:)') - P(3,:) * M * (P(3,:)') ;
    err4=P(1,:) * M * (P(3,:)') ;
    err5=P(2,:) * M * (P(3,:)') ;
    err6=P(1,:) * M * (P(2,:)') ;
    
%     err1
%     err2
%     err3
%     err4
%     err5
%     err6
    
    err= err+ abs(err1)+ abs(err2)+ abs(err3)+ abs(err4)+ abs(err5)+ abs(err6);
end



end

