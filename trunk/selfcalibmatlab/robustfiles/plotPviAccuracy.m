function []= plotPviAccuracy(beginp, endp, repeatEach, repeatRatio)


t=beginp:((endp-beginp)/10):endp;
npts=200;
AlgNames={ 'cookUpdate','compete'};
AlgFuncs={5,9};

s=7;

for z=1:size(AlgFuncs,2)
    data{z}=zeros(3, size(t,2));
end

for i=1:size(t,2)
display([' at noise ratio ' num2str(t(1,i))]);
    for p=1:repeatRatio

        [ Fgt,k1,k2,corrs,corrsclean, inlierOutlier, I1, I2,  R1, t1,R2,t2 ] = generateCorrsforF(npts, t(1,i), 1, 's','merton1' );
        [F1,LeverageAll] = fundmatrix(corrs);
        startpvis=0.5*ones(npts,1); 
        
        for q=1:repeatEach


            for z=1:size(AlgFuncs,2)
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
            end
        end

    end

end

for z=1:size(AlgFuncs,2)
    data{z}=data{z}/repeatEach;
end
why are somer numbers bigger than 1

styles={'-.or' ,'-.xg', '-.+b', '-.*y', '-.vr' ,'-..c','g--'};
categories={'inlier error','outlier error','total error'};

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