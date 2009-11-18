function [ F ] = generateF( fdiff, skew, ar,centerdeviation )


 ps  = generatePs( fdiff, skew, ar,centerdeviation, 2,1 );

P1=ps{1,1};
P2=ps{1,2};


F = vgg_F_from_P(P1, P2);







end