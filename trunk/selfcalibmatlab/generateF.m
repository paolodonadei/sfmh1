function [ F, ks,ps ] = generateF( fdiff, skew, ar,centerdeviation,silent )


[ ps ,myks ] = generatePs( fdiff, skew, ar,centerdeviation, 2,1,silent );

P1=ps{1,1};
P2=ps{1,2};


F = vgg_F_from_P(P1, P2);

ks=myks;





end