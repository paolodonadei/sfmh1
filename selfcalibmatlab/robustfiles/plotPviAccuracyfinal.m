function []= plotPviAccuracyfinal(beginp, endp,  repeatRatio)


t=beginp:((endp-beginp)/10):endp;
npts=200;
AlgNames={ 'cookUpdate','compete9','compete10'};
AlgFuncs={5,9,10};

s=7;

for z=1:size(AlgFuncs,2)
    data{z}=zeros(4, size(t,2));
end


for i=1:size(t,2)
    display([' at noise ratio ' num2str(t(1,i))]);
    for p=1:repeatRatio

        [ Fgt,k1,k2,corrs,corrsclean, inlierOutlier, I1, I2,  R1, t1,R2,t2 ] = generateCorrsforF(npts, t(1,i), 1, 's','merton1' );


        for z=1:size(AlgFuncs,2)
            clear F  iters pvisot
            [F, iters,pvisot] = fundmatrixrobustgeneral(corrs,AlgFuncs{z});
            [bestInliers, bestF, residuals, meaner,varer,meder,numins] = sampsonF(F, corrsclean, 1.96*1.96);
            [errorin, errorout,errorstot, errors] = pvifitness(inlierOutlier',pvisot);
            data{z}(1,i)=data{z}(1,i)+iters;
            data{z}(2,i)=data{z}(2,i)+errorin;
            data{z}(3,i)=data{z}(3,i)+errorout;
            data{z}(4,i)=data{z}(4,i)+meaner;


        end
    end

end



for z=1:size(AlgFuncs,2)
    data{z}=data{z}./(repeatRatio);
end


styles={'-.or' ,'-.xg', '-.+b', '-.*y', '-.vr' ,'-..c','g--'};
categories={'iterations ','error inliers','error outliers', ' mean error'};

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