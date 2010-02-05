function [ F, ks,ps ] = generateF( fdiff, skew, ar,centerdeviation,silent,number,noiselevel,numCorruptFs )




[ ps ,myks ] = generatePs( fdiff, skew, ar,centerdeviation, number,1,silent,0 );

if (nargin <7)

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

else

    if(noiselevel>1)
        disp('noise level is hjigher than 1');
    end
    if(numCorruptFs>((number*(number-1))/2))
        disp('demanded more corrupt Fs than there are Fs');
    end



    numCorrs=200;
    count=0;
    for i=1:number
        for j=1:i
            if(i~=j)
                count=count+1;

                if(count<=numCorruptFs)
                    clear x1 x2;
                    x1=zeros(3, numCorrs);
                    x2=zeros(3, numCorrs);
                    X=zeros(4, numCorrs);
                    for q=1:numCorrs
                        X(1,q)=rand()*1000 -500;
                        X(2,q)=rand()*1000 -500;
                        X(3,q)=rand()*1000 -500;
                        X(4,q)=1;

                        x1(:,q)=ps{1,i}*X(:,q);

                        if(q<(numCorrs*noiselevel))

                            x2(1,q)=rand()*512;
                            x2(2,q)=rand()*512;
                            x2(3,q)=1;
                        else
                            x2(:,q)=ps{1,j}*X(:,q);
                        end
                    end

                    [F{count}, e1, e2] = fundmatrix(x1, x2);
                else
                    F{count}=vgg_F_from_P(ps{1,i},ps{1,j});
                end
            end
        end
    end


end



ks=myks;





end