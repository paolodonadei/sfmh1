function  [M] =chooseFinalQ(QS )

M=eye(4,4);
flagfound=0;



for k=1:size(QS,1)
    QM=QS{1,k};
    
    isposdef = matposdef( QM);
    % Q= eig(M)
    
    if(isposdef==1)
        M=QM
        disp(['chose S with index ' num2str(k)]);
        flagfound=1;
        break;
    end
    
end

if(flagfound==0)
    
    disp('no Qs were found to be positive definite');
    QS{1,1}
    
    
    
end



end
