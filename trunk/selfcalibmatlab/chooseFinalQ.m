function  [MS] =chooseFinalQ(QS )

MS={eye(4,4)};
flagfound=0;
j=1;


for k=1:size(QS,2)
    QM=QS{1,k};
    
    isposdef = matposdef( QM);
    % Q= eig(M)
    
    if(isposdef==1)
        M=QM;
        disp(['chose S with index ' num2str(k)]);
        flagfound=1;
        MS{1,j}=M;
        j=j+1;
        break;  % i am disregarding the subsequent M
    end
    
end

if(flagfound==0)
    QS{1,1}
    disp('no Qs were found to be positive definite');
    
    MS={};
    
    
end



end
