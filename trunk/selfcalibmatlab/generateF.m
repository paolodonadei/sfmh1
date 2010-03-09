function [ F, ks,ps , badpoints ] = generateF( fdiff, skew, ar,centerdeviation,silent,number,noiselevel,numCorruptFs )

numCorrs=200;

badpoints=zeros(numCorrs,((number*(number-1))/2) );


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




    count=0;
    for i=1:number
        for j=1:i
            if(i~=j)
                count=count+1;

                if(count<=numCorruptFs)
                    clear x1 x2 X;
                    x1=zeros(3, numCorrs);
                    x2=zeros(3, numCorrs);
                    X=zeros(4, numCorrs);
                    for q=1:numCorrs
                        X(1,q)=rand()*1000 -500;
                        X(2,q)=rand()*1000 -500;
                        X(3,q)=rand()*1000 -500;
                        X(4,q)=1;

                        x1(:,q)=ps{1,i}*X(:,q);
                        x2(:,q)=ps{1,j}*X(:,q);

                        x2(1,q)=x2(1,q)/x2(3,q);
                        x2(2,q)=x2(2,q)/x2(3,q);
                        x2(3,q)=1;


                        x1(1,q)=x1(1,q)/x1(3,q);
                        x1(2,q)=x1(2,q)/x1(3,q);
                        x1(3,q)=1;


                        if(q<(numCorrs*noiselevel))


                            noise1=(rand()*512);
                            noise2=(rand()*512);

                            if(rand()<0.5)
                                x2(1,q)=noise1; % outlier generation, this is whack and important
                                x2(2,q)=noise2;
                           
                            else
                                x1(1,q)=noise1; % outlier generation, this is whack and important
                              
                                x1(2,q)=noise2;
                            end





                        end
                    end

                    [F{count}, e1, e2] = fundmatrix(x1, x2);

%                     xbad = PeterSturmSelf(F{count},512,512);
%                     Fgood=vgg_F_from_P(ps{1,i},ps{1,j});
%                     xgood = PeterSturmSelf(Fgood,512,512);
% 
%                     xbad
%                     xgood
%                     myks{1,1}(1,1)

                else
                    F{count}=vgg_F_from_P(ps{1,i},ps{1,j});
                end

                % measure noise
                %                 clear x1 x2 X;
                %                 x1=zeros(3, numCorrs);
                %                 x2=zeros(3, numCorrs);
                %                 X=zeros(4, numCorrs);
                %
                %                 for q=1:numCorrs
                %                     X(1,q)=rand()*1000 -500;
                %                     X(2,q)=rand()*1000 -500;
                %                     X(3,q)=rand()*1000 -500;
                %                     X(4,q)=1;
                %
                %                     x1(:,q)=ps{1,i}*X(:,q);
                %                     x2(:,q)=ps{1,j}*X(:,q);
                %
                %                     l1=(F{count})*x1(:,q);
                %                     dn=(l1(1,1)*(x2(1,q)/x2(3,q)))+(l1(2,1)*(x2(2,q)/x2(3,q)))+l1(3,1);
                %                     dn=dn*dn;
                %                     d1=(l1(1,1)*l1(1,1))+(l1(2,1)*l1(2,1));
                %                     e1=sqrt(dn/d1);
                %
                %
                %                     l2=(F{count}')*x2(:,q);
                %                     dn=(l2(1,1)*(x1(1,q)/x1(3,q)))+(l2(2,1)*(x1(2,q)/x1(3,q)))+l2(3,1);
                %                     dn=dn*dn;
                %                     d2=(l2(1,1)*l2(1,1))+(l2(2,1)*l2(2,1));
                %                     e2=sqrt(dn/d2);
                %
                %
                %
                %
                %                     badpoints(q,count)=abs(e1)+ abs(e2);
                %                 end

            end
        end
    end


end



ks=myks;





end