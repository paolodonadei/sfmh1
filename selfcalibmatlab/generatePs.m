function [ ps ] = generatePs( fdiff, skew, ar,centerdeviation, numps )
%if fdiff is 1 then all Fs will be different, if 0 otehrwise,
%skew is just the max skew allowed, if its zero them all skews is zero
%ar is the aspect ratio
% centerdeviation is how much the center will deviate from the image center
%numps is the number of the projection matrices

format long g;

rand('state', sum(100*clock));

WIDTH=512;
HEIGHT=512;

a = -40; b = 40;

ps = cell(1,numps) ;

dirname=['projFolder' num2str(sum(round(100*clock)))];
mkdir(dirname);
overallF=300+(rand()*900); % if fdiff is 0 then they are all set to this
curentF=0;

T=zeros(3,1);
R=eye(3,3);
for i=1:numps


    if(i~=1)
        T= a + (b-a) * rand(3,1);
        R= rotationmat3D((-pi + (pi*2) * rand()),[(rand()*2-1) (rand()*2-1) (rand()*2-1)]);
    end

    K=eye(3);
    K(1,2)=rand()*skew;

    if(fdiff==0)
        curentF=overallF;
    else
        curentF=300+(rand()*900);
    end


    K(1,1)=curentF;
    K(2,2)=K(1,1)*ar;

    K(1,3)=(WIDTH/2)+rand()*centerdeviation;
    K(2,3)=(HEIGHT/2)+rand()*centerdeviation;

    P=K*[R -R*T];
    ps{1,i}=P;
    
     disp(['forcal length ' num2str(i) ' is equal to: ' num2str(curentF)]);
    %now write all this to file
    save([dirname '/K' num2str(i) '.txt'], 'K','-ascii', '-double');
    save([dirname '/R' num2str(i) '.txt'], 'R','-ascii', '-double');
    save([dirname '/T' num2str(i) '.txt'], 'T','-ascii', '-double');
    save([dirname '/P' num2str(i) '.txt'], 'P','-ascii', '-double');
end













end