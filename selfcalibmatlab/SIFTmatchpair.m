function [ F,corrs, I1, I2, I1motion ] = SIFTmatchpair(fname1,fname2 )

I1 = imread(fname1) ;
[m,n,p]=size(I1);
if(p>1)
   I1g= rgb2gray(I1);
else
    I1g= I1;
end
I1f = double(I1g/256) ;
[frames1,descriptors1] = sift(I1f, 'Verbosity', 0) ;
    imshow(I1), hold on, h1=plot(frames1(1,:)',frames1(2,:)','r+');
   imshow(I2), hold on, h2=plot(frames2(1,:)',frames2(2,:)','r+');
I2 = imread(fname2) ;
[m,n,p]=size(I2);
if(p>1)
   I2g= rgb2gray(I2);
else
    I2g= I2;
end
I2f = double(I2g/256) ;
[frames2,descriptors2] = sift(I2f, 'Verbosity',0) ;





matches = siftmatch(descriptors1, descriptors2) ;

 h=plotmatches(I1g,I2g,frames1,frames2,matches);
 saveas(h,'rob','png') ;
 I1motion=imread('rob.png');


