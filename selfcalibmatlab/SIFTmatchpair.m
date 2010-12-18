function [ corrs, I1, I2] = SIFTmatchpair(fname1,fname2 )

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
figure
I2 = imread(fname2) ;
[m,n,p]=size(I2);
if(p>1)
   I2g= rgb2gray(I2);
else
    I2g= I2;
end
I2f = double(I2g/256) ;
[frames2,descriptors2] = sift(I2f, 'Verbosity',0) ;
   imshow(I2), hold on, h2=plot(frames2(1,:)',frames2(2,:)','r+');



figure
matches = siftmatch(descriptors1, descriptors2) ;

 h=plotmatches(I1g,I2g,frames1,frames2,matches);

 
 corrs=zeros(4,size(matches,2));
 
 for i=1:size(matches,2)
    
     corrs(1,i)= frames1(1,matches(1,i))  ;
     corrs(2,i)= frames1(2,matches(1,i))   ;
     
     corrs(3,i)=  frames2(1,matches(2,i))  ;
     corrs(4,i)=  frames2(2,matches(2,i))  ;
     
     
 end


