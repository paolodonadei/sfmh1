%vgg_F_from_7pts_2img  Computes fundamental matrix from 7 points across 2 images.
%
%   [P,X] = vgg_F_from_7pts_2img(x), where
%      x ... double(3,7,2) or cell{2} of double(3,7), 7 homogeneous points in 2 images
%      F ... double(3,3), fundamental matrix
%   There are 0 to 3 solutions for F. Solutions are pruned by requirement that
%   scalars s in all equations s*cross(e1,x1)==F*x2 are positive.
%   In case of multiple solutions, F has one dimension
%   more such that F(:,:,n) is the n-th solution.
%
%   Also the form F = vgg_F_from_7pts_2img(x1,x2) is accepted.

function F = vgg_F_from_7pts_2img2(corrs)

if(size(corrs,1)==4)
    x1 = corrs(1:2,:);

    x2 = corrs(3:4,:);

    x1=[x1 ; ones(1,7)];
    x2=[x2 ; ones(1,7)];
elseif (size(corrs,1)==6)
    x1 = corrs(1:3,:);
    x2 = corrs(4:6,:);
else
    display('wrong in 7 points');

end

if any(size(x1)~=[3 7]) | any(size(x2)~=[3 7])
    error('Wrong size of input points.');
end

% Linear step
A = vgg_vec_swap(x1,x2)';


[u,s,v] = svd(A,0);
FF{1} = reshape(v(:,end-1),[3 3]);
FF{2} = reshape(v(:,end  ),[3 3]);

% Solving cubic equation and getting 1 or 3 solutions for F
a = vgg_singF_from_FF(FF);
F = [];
for i = 1:length(a)
    Fi = a(i)*FF{1} + (1-a(i))*FF{2};
    %   for n = 1:7, disp(norm(x1(:,n)'*Fi*x2(:,n))), end  % test code
    if signs_OK(Fi,x1,x2)
        F = cat(3, F, Fi);
    end
end

if isempty(F)
    F = [];
    return;
end
[m,n,q]=size(F);

if(q>1)
    FB={};
    for k=1:q

        FB{k}=F(:,:,k);
        FB{k}=FB{k}';
    end
    clear F
    F=FB;
else
    F=F';
end


return

%%%%%%%%%%%%%%%%%%%%%%%%%

% Checks sign consistence of F and x
function OK = signs_OK(F,x1,x2)
[u,s,v] = svd(F');
e1 = v(:,3);
l1 = vgg_contreps(e1)*x1;
s = sum( (F*x2) .* l1 );
OK = all(s>0) | all(s<0);
return