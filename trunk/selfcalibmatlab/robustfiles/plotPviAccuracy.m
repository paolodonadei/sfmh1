function []= plotPviAccuracy(beginp, endp, repeatEach, repeatRatio)


t=beginp:((endp-beginp)/10):endp;
npts=200;
AlgNames={ 'cookUpdate','compete9','compete10'};
AlgFuncs={5,9,10};

s=7;

for z=1:size(AlgFuncs,2)
    data{z}=zeros(6, size(t,2));
end



     startpvis=0.5*ones(npts,1);
for i=1:size(t,2)
    display([' at noise ratio ' num2str(t(1,i))]);
    for p=1:repeatRatio
        
        [ Fgt,k1,k2,corrs,corrsclean, inlierOutlier, I1, I2,  R1, t1,R2,t2 ] = generateCorrsforF(npts, t(1,i), 1, 's','merton1' );
        [F1,LeverageAll] = fundmatrix(corrs);
    
        for z=1:size(AlgFuncs,2)
           
        initPvisreUsed=LeverageAll;
        clear persistentPvis;
          persistentPvis=0.5*ones(npts,1);
            for q=1:repeatEach
                
                
                
                clear M;
                M=[];
                while(size(M,1)==0)
                    ind =randsampleengine(npts, s,startpvis,5);
                    if(s==7)
                        M = vgg_F_from_7pts_2img2(corrs(:,ind));
                    elseif(s>7)
                        
                        M =  fundmatrix(corrs(:,ind));
                    else
                        display('wrong S');
                        return;
                    end
                end
                
                [bestInliers, bestF, residuals, meaner,varer,meder,numins] = sampsonF(M, corrs,  1.96*1.96);
                [outpvis, outinitpvi] = generalpviUpdate(AlgFuncs{z},LeverageAll,startpvis,residuals,1.96*1.96,bestInliers,corrs,1,10000);
                [errorin, errorout,errors] = pvifitness(inlierOutlier',outpvis);
                data{z}(1,i)=data{z}(1,i)+errorin;
                data{z}(2,i)=data{z}(2,i)+errorout;
                data{z}(3,i)=data{z}(3,i)+errors;
                
       % resampled with initial pvis %
                          clear M;
                         
                M=[];
                while(size(M,1)==0)
                    ind =randsampleengine(npts, s,startpvis,5);
                    if(s==7)
                        M = vgg_F_from_7pts_2img2(corrs(:,ind));
                    elseif(s>7)
                        
                        M =  fundmatrix(corrs(:,ind));
                    else
                        display('wrong S');
                        return;
                    end
                end
                
                [bestInliers, bestF, residuals, meaner,varer,meder,numins] = sampsonF(M, corrs,  1.96*1.96);
                [persistentPvis, initPvisreUsed] = generalpviUpdate(AlgFuncs{z},initPvisreUsed,persistentPvis,residuals,1.96*1.96,bestInliers,corrs,1,10000);
                [errorinp, erroroutp,errorsp] = pvifitness(inlierOutlier',persistentPvis);

                data{z}(4,i)=data{z}(4,i)+errorinp;
                data{z}(5,i)=data{z}(5,i)+erroroutp;
                data{z}(6,i)=data{z}(6,i)+errorsp;
              
            end
        end
        
    end
    
end

for z=1:size(AlgFuncs,2)
    data{z}=data{z}./(repeatEach*repeatRatio);
end


styles={'-.or' ,'-.xg', '-.+b', '-.*y', '-.vr' ,'-..c','g--'};
categories={'inlier error','outlier error','total error', ' resampled inlier error', ' resampled outlier error', ' resampled total error'};

for i=1:size(categories,2)
    figure;
    hold
    for z=1:size(AlgFuncs,2)
        plot(t,data{z}(i,:),styles{z});
    end
    xlabel(['x outlier ratio']);       %  add axis labels and plot title
    ylabel(['y ' categories{i}]);
    title([ ' plot of ' categories{i}  ' versus error ratio']);
    legend(AlgNames);
    
    hold
end



end