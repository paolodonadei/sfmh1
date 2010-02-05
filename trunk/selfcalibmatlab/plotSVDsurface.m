function [X,Y,Z]=plotSVDsurface(F,ks,leaveF)
% F is the cell containing the fundamental matrices
% j is the fundamental matrix that shouldnt be used

% we assume all funds have the same intrics

if (nargin == 2)
    leaveF=0;
end


xcenter=floor(ks{1,1}(1,3));
ycenter=floor(ks{1,1}(2,3));

xyrange=100;

fbegin=floor(abs(ks{1,1}(1,1)-200));
fend=floor(abs(ks{1,1}(1,1)+200));

[m,numFs]=size(F);

errOC=zeros(2*xyrange+1,2*xyrange+1);
errF=zeros(fend-fbegin+1);

for i=(xcenter-xyrange):1:(xcenter+xyrange)
    for j=(ycenter-xyrange):1:(ycenter+xyrange)
        for k=1:numFs
            if(k~=leaveF)
                errOC(i-(xcenter-xyrange)+1 ,j-(ycenter-xyrange)+1  ) =errOC(i-(xcenter-xyrange)+1 ,j-(ycenter-xyrange)+1  )+ computerEssentialErrorSVD([ks{1,1}(1,1) i j],F{1,k});
            end
        end
    end
end


for i=fbegin:1:fend
    for k=1:numFs
        if(k~=leaveF)
            errF(i-fbegin+1)=errF(i-fbegin+1)+computerEssentialErrorSVD([i ks{1,1}(1,3) ks{1,1}(2,3) ],F{1,k});
        end
    end
end
for i=fbegin:1:fend
    if(errF(i-fbegin+1)>1)
        errZ(i-fbegin+1)  =1;
    else
        errZ(i-fbegin+1)  =errF(i-fbegin+1);

    end


end

[X,Y] = meshgrid( (xcenter-xyrange):1:(xcenter+xyrange),(ycenter-xyrange):1:(ycenter+xyrange) );
mesh(X,Y,errOC) ;
title('energy function versus optical center');
figure

plot(fbegin:1:fend,errF);
title('energy function versus focal length');

figure

plot(fbegin:1:fend,errZ);
title('cropped energy function versus focal length');


Z=errOC;
end