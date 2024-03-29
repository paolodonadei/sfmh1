function [F] = computerEssentialErrorSVDNFrames(x,MYF)
%this function , given a camera center and a focal length and a fundamental
%matrix computes the error with respect to a fundamental matrix
[m,n]=size(MYF);

F=0;
for i=1:n

    T = computerEssentialErrorSVD(x,MYF{1,i});

    F=F+T;
end
if(n>0)
    F=F/n; %is this right? this is important
else
    disp(['why is n zero in the objective function? did the diagnostics method remove all the Fs']);
    MYF
end



end

