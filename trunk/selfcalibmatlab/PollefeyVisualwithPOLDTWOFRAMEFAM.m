function [ x ] = PollefeyVisualwithPOLDTWOFRAMEFAM( F,w,h )
%code conforms to the marr prize paper but with full parametrization of the
%Q so we solve for the 10 variables
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
%http://www.robots.ox.ac.uk/~vgg/hzbook/code/
%http://www.csse.uwa.edu.au/~pk/research/matlabfns/
% data:   http://www.robots.ox.ac.uk/~vgg/data/data-mview.html
f1=0;
f2=0;
x=[0 0];

P=PsfromF( F );
[m n ]=size(P);
numFrames=n;


P_in=P;



if(m>1)
    disp([ 'the size of the input cell is wrong']);
end


%create the normalizing matrix
[ K_norm, K_norm_inv ] = findNormalizingK(w,h);

% K_norm=eye(3,3);
% K_norm_inv=K_norm;

%perform normalization
for i=1:numFrames
    P_in{1,i}=K_norm_inv*P_in{1,i};
    %   P_in{1,i}=P_in{1,i}/norm(P_in{1,i},'fro');
end


%now form A

 [A,b]  = formAunknownF( P_in );


% take Q out of the svd
[U,S,V] = svd(A);
%disp([ 'last svd is ' num2str(S(10,10)) ' and the condition number is (it should be near 1) ' num2str(cond(A)) ' and the rank is ' num2str(rank(A))] );


Q1=myFormatvectoQ(V(:,10));
Q2=myFormatvectoQ(V(:,9));

S=findSolsfromQ(Q1,Q2);

QH_1=normalizeSetRank(Q1+ S(1)*Q2);
QH_2=normalizeSetRank(Q1+ S(2)*Q2);
QH_3=normalizeSetRank(Q1+ S(3)*Q2);
QH_4=normalizeSetRank(Q1+ S(4)*Q2);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
QS={QH_1 QH_2 QH_3 QH_4};




for k=1:4
    M=QS{1,k};
    ST=S(k);
    isposdef = matposdef( M);
   % Q= eig(M)

    if(isposdef==1)
        % disp([' the alpha was ' num2str(ST) ' number 1']);


        K1=findKfromPQ(K_norm,P_in{1,1},M);
        K2=findKfromPQ(K_norm,P_in{1,2},M);
       % disp([' frame : ' num2str(1) ' the focal length is (K[0][0]) ' num2str(K1(1,1)) ' or (K[1][1])' num2str(K1(2,2))]);
       % disp([' frame : ' num2str(2) ' the focal length is (K[0][0]) ' num2str(K2(1,1)) ' or (K[1][1])' num2str(K2(2,2))]);
%     P_in{1,1}
%     M
%     p=planeInfFromQ(M)
%     K1
%     K2
        f1=K1(1,1);
        f2=K2(1,1);
        break;
    end

end


x=[f1   f2];

end





