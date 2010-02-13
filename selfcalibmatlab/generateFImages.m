function [ MF ] = generateFImages( IMS,noiselevel, numCorruptFs )

% a lot of the things here are taken from peter kovesi
%http://www.csse.uwa.edu.au/~pk/research/matlabfns/Robust/example/index.html


thresh = 500;   % Harris corner threshold
nonmaxrad = 3;  % Non-maximal suppression radius
w = 7;    % Window size for correlation matching
t = .001;  % Distance threshold for deciding outliers
showgraphs=0;
count=0;

[m,numIs]=size(IMS);

for i=1:numIs
    for j=1:i
        if(i~=j)
            count=count+1;
            clear cim1 r1 c1 cim2 r2 c2 m1 m2 F  inliers im1 im2 m1 m2 x1 x2;

            [vm,vn,vj]=size(IMS{1,i});

            if(vj>1)
                im1 = 	rgb2gray(IMS{1,i});
                im2 = 	rgb2gray(IMS{1,j});
            else
                im1 = 	(IMS{1,i});
                im2 = 	(IMS{1,j});
            end

            [cim1, r1, c1] = harris(im1, 1, thresh, 3);

            [cim2, r2, c2] = harris(im2, 1, thresh, 3);



            [m1,m2] = matchbycorrelation(im1, [r1';c1'], im2, [r2';c2'], w);




            x1 = [m1(2,:); m1(1,:); ones(1,length(m1))];
            x2 = [m2(2,:); m2(1,:); ones(1,length(m1))];

            [F, inliers] = ransacfitfundmatrix(x1, x2, t);



            l2 = F*x1;    % Epipolar lines in image2
            l1 = F'*x2;   % Epipolar lines in image1

            if(showgraphs==1)
                show(im1,1), hold on, plot(c1,r1,'r+');
                show(im2,2), hold on, plot(c2,r2,'r+');

                % Display putative matches
                show(im1,3), set(3,'name','Putative matches'), hold on
                for n = 1:length(m1);
                    line([m1(2,n) m2(2,n)], [m1(1,n) m2(1,n)])
                end
                % Display both images overlayed with inlying matched feature points
                show(double(im1)+double(im2),4), set(4,'name','Inlying matches'), hold on
                plot(m1(2,inliers),m1(1,inliers),'r+');
                plot(m2(2,inliers),m2(1,inliers),'g+');

                for n = inliers
                    line([m1(2,n) m2(2,n)], [m1(1,n) m2(1,n)],'color',[0 0 1])
                end

%                 for n = inliers
%                     figure(1), clf, show(im1,1), hold on, plot(x1(1,n),x1(2,n),'r+');
%                     hline(l1(:,n));
%                     figure(2), clf, show(im2,2), hold on, plot(x2(1,n),x2(2,n),'r+');
%                     hline(l2(:,n));
%                     fprintf('hit any key to see next point\r'); pause
%                 end


                fprintf('hit any key to see next point\r'); pause
            end




            MF{count}=F;
        end
    end
end


end