G=F;

gfocal=(ks{1,1}(1,1)+ks{1,1}(2,2))/2;
gocx=  ks{1,1}(1,3);
gocy=ks{1,1}(2,3)  ;
width=512; height=512;
width=1024; height=768;

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


errps=abs(sln(1)-gfocal);
errnfram=abs(fcln(1)-gfocal);
errtwofram=abs(fclt(1)-gfocal);
errdiagn=abs(fcld(1)-gfocal);
errM=abs(fclm(1)-gfocal);
errR=abs(fclR(1)-gfocal);


errnframOC=sqrt(((centerlocn(1)-gocx)^2)+((centerlocn(2)-gocy)^2));
errtwoframOC=sqrt(((centerloct(1)-gocx)^2)+((centerloct(2)-gocy)^2));
errdiagOC=sqrt(((centerlocdiag(1)-gocx)^2)+((centerlocdiag(2)-gocy)^2));
errMOC=sqrt(((centerlocM(1)-gocx)^2)+((centerlocM(2)-gocy)^2));
errROC=sqrt(((centerlocR(1)-gocx)^2)+((centerlocR(2)-gocy)^2));


disp(['hough algorithm had error in f of ' num2str(errtwofram) ' and in optical center of ' num2str(errtwoframOC) ' with time ' num2str(TtElapsed)   ]);
disp(['nonrobust algorithm had error in f of ' num2str(errnfram) ' and in optical center of ' num2str(errnframOC)   ' with time ' num2str(NtElapsed) ]);
disp(['peter sturm algorithm had error in f of ' num2str(errps) ' and in optical center of ' num2str(200)   ' with time ' num2str(PtElapsed) ]);
disp(['ransac algorithm had error in f of ' num2str(errR) ' and in optical center of ' num2str(errROC)    ' with time ' num2str(RtElapsed)]);
disp(['diagnostics algorithm had error in f of ' num2str(errdiagn) ' and in optical center of ' num2str(errdiagOC)   ' with time ' num2str(DtElapsed) ]);
disp(['m-estimator algorithm had error in f of ' num2str(errM) ' and in optical center of ' num2str(errMOC)   ' with time ' num2str(MtElapsed) ]);