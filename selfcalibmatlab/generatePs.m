function [ ps ,ks ] = generatePs( fdiff, skew, ar,centerdeviation, numps,projective,silent )
%if fdiff is 1 then all Fs will be different, if 0 otehrwise,
%skew is just the max skew allowed, if its zero them all skews is zero
%ar is the aspect ratio
% centerdeviation is how much the center will deviate from the image center
%numps is the number of the projection matrices
projective=1;
format long g;

rand('state', sum(100*clock));

WIDTH=512;
HEIGHT=512;

a = -40; b = 40;

ps = cell(1,numps) ;
ks = cell(1,numps) ;

overallF=300+(rand()*900); % if fdiff is 0 then they are all set to this
curentF=0;

T=zeros(3,1);
R=eye(3,3);
prevF=overallF;

for i=1:numps


    if(i~=1)
        T= a + (b-a) * rand(3,1);
        R= rotationmat3D((-pi + (pi*2) * rand()),[(rand()*2-1) (rand()*2-1) (rand()*2-1)]);
    end

    K=eye(3);
    K(1,2)=skew;

    if(numps==2)  %this might not make sense, but for 2 frames i dont want the focal length difference to be a random variable
        curentF=abs(prevF+(fdiff));
    else
        curentF=abs(prevF+((rand()*fdiff)-(fdiff/2)));
    end

    prevF=curentF;
    K(1,1)=curentF;
    K(2,2)=K(1,1)*ar;

    if(numps==2)  %this might not make sense, but for 2 frames i dont want the deviation from center to be a random variable
        K(1,3)=(WIDTH/2)+centerdeviation;
        K(2,3)=(HEIGHT/2)+centerdeviation;
    else
        K(1,3)=(WIDTH/2)+rand()*centerdeviation;
        K(2,3)=(HEIGHT/2)+rand()*centerdeviation;
    end


    P=K*[R -R*T];

    ps{1,i}=P;
    ks{1,i}=K;

   
    %now write all this to file

    if(silent~=1)
         disp(['forcal length ' num2str(i) ' is equal to: K[0][0] ' num2str(K(1,1)) ' or K[1][1] ' num2str(K(2,2))]);
        dirname=['projFolder' num2str(sum(round(100*clock)))];
        mkdir(dirname);
        save([dirname '/K' num2str(i) '.txt'], 'K','-ascii', '-double');
        save([dirname '/R' num2str(i) '.txt'], 'R','-ascii', '-double');
        save([dirname '/T' num2str(i) '.txt'], 'T','-ascii', '-double');
        save([dirname '/P' num2str(i) '.txt'], 'P','-ascii', '-double');
    end


end













end