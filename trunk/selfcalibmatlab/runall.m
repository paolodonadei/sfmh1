function [t] = runall(G,ks,width,height)
% 
% G=F;
% 
% %width=512; height=512;
% width=1024; height=768;

if (nargin == 2)
    w=512;
    h=512;
end

tic; 
[fcln, centerlocn]        = S2nonlinsolveEssNfram(G,width,height);
NtElapsed=toc;

tic; 
[fclt, centerloct]        = S2nonlinsolveEsstwofram(G,width,height);
TtElapsed=toc;

tic; 
[fcld, centerlocdiag]     = S2nonlinsolveEssNframdiagnostics(G,width,height);
DtElapsed=toc;

tic; 
[fclm, centerlocM]        = S2nonlinsolveEssNframestimator(G,width,height);
MtElapsed=toc;

tic; 
[ sln, centerloc ]        = PeterSturmSelfRobust( G,width,height );
PtElapsed=toc;

tic; 
[ fclR, centerlocR ]      =  S2nonlinsolveEssRansac( G,width,height );
RtElapsed=toc;


[errps, errOCps  ] = calcSelfCalibError(sln, centerloc,ks);

[errnfram,errnframOC  ] = calcSelfCalibError(fcln, centerlocn,ks);

[errtwofram, errtwoframOC  ] = calcSelfCalibError(fclt, centerloct,ks);

[errdiagn, errdiagOC  ] = calcSelfCalibError(fcld, centerlocdiag,ks);

[errM, errMOC  ] = calcSelfCalibError(fclm, centerlocM,ks);

[errR, errROC ] = calcSelfCalibError(fclR, centerlocR,ks);



disp(['hough algorithm had error in f of ' num2str(errtwofram) ' and in optical center of ' num2str(errtwoframOC) ' with time ' num2str(TtElapsed)   ]);
disp(['nonrobust algorithm had error in f of ' num2str(errnfram) ' and in optical center of ' num2str(errnframOC)   ' with time ' num2str(NtElapsed) ]);
disp(['peter sturm algorithm had error in f of ' num2str(errps) ' and in optical center of ' num2str(200)   ' with time ' num2str(PtElapsed) ]);
disp(['ransac algorithm had error in f of ' num2str(errR) ' and in optical center of ' num2str(errROC)    ' with time ' num2str(RtElapsed)]);
disp(['diagnostics algorithm had error in f of ' num2str(errdiagn) ' and in optical center of ' num2str(errdiagOC)   ' with time ' num2str(DtElapsed) ]);
disp(['m-estimator algorithm had error in f of ' num2str(errM) ' and in optical center of ' num2str(errMOC)   ' with time ' num2str(MtElapsed) ]);

t=1;

end