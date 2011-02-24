% FUNDMATRIX - computes fundamental matrix from 8 or more points
%
% Function computes the fundamental matrix from 8 or more matching points in
% a stereo pair of images.  The normalised 8 point algorithm given by
% Hartley and Zisserman p265 is used.  To achieve accurate results it is
% recommended that 12 or more points are used
%
% Usage:   [F, e1, e2] = fundmatrix(x1, x2)
%          [F, e1, e2] = fundmatrix(x)
%
% Arguments:
%          x1, x2 - Two sets of corresponding 3xN set of homogeneous
%          points.
%
%          x      - If a single argument is supplied it is assumed that it
%                   is in the form x = [x1; x2]
% Returns:
%          F      - The 3x3 fundamental matrix such that x2'*F*x1 = 0.
%          e1     - The epipole in image 1 such that F*e1 = 0
%          e2     - The epipole in image 2 such that F'*e2 = 0
%

% Copyright (c) 2002-2005 Peter Kovesi
% School of Computer Science & Software Engineering
% The University of Western Australia
% http://www.csse.uwa.edu.au/
%
% Permission is hereby granted, free of charge, to any person obtaining a copy
% of this software and associated documentation files (the "Software"), to deal
% in the Software without restriction, subject to the following conditions:
%
% The above copyright notice and this permission notice shall be included in
% all copies or substantial portions of the Software.
%
% The Software is provided "as is", without warranty of any kind.

% Feb 2002  - Original version.
% May 2003  - Tidied up and numerically improved.
% Feb 2004  - Single argument allowed to enable use with RANSAC.
% Mar 2005  - Epipole calculation added, 'economy' SVD used.
% Aug 2005  - Octave compatibility

function [F,e1,e2] = fundmatrix(varargin)

[x1, x2, npts,weights,ended here add options for using nonline with this and try to see if its better with random sampling or with the final fit 
    also weigh the residuals wirth the weights for nonlin in case you wanna use it in the end] = checkargs(varargin(:));

origX1=x1;
origX2=x2;

Octave = exist('OCTAVE_VERSION') ~= 0;  % Are we running under Octave?

% Normalise each set of points so that the origin
% is at centroid and mean distance from origin is sqrt(2).
% normalise2dpts also ensures the scale parameter is 1.
[x1, T1] = normalise2dpts(x1);
[x2, T2] = normalise2dpts(x2);

% Build the constraint matrix
A = [x2(1,:)'.*x1(1,:)'   x2(1,:)'.*x1(2,:)'  x2(1,:)' ...
    x2(2,:)'.*x1(1,:)'   x2(2,:)'.*x1(2,:)'  x2(2,:)' ...
    x1(1,:)'             x1(2,:)'            ones(npts,1) ];

for i=1:size(A,1)
   A(i,:)= A(i,:)*weights(i,1);
end

[U,D,V] = svd(A,0); % Under MATLAB use the economy decomposition


% Extract fundamental matrix from the column of V corresponding to
% smallest singular value.
F = reshape(V(:,9),3,3)';

% Enforce constraint that fundamental matrix has rank 2 by performing
% a svd and then reconstructing with the two largest singular values.
[U,D,V] = svd(F,0);
F = U*diag([D(1,1) D(2,2) 0])*V';







global corrsclean;
[bestInliers, bestF, residuals, meaner,varer,meder,numins] = sampsonF(T2'*F*T1, corrsclean );

display([' before nonlin error was ' num2str(meaner)]);

options = optimset('Display','off','Diagnostics','off','Algorithm','interior-point','TolFun',1.0e-76,'TolCon',1.0e-76,'TolX',1.0e-76);
fo = fmincon('FMatrixNonLinError',[F(1,:) F(2,:) F(3,:)]',[],[],[],[],[],[],@torr_nonlcon_f2x2,options,[x1 ; x2],1.96*1.96);

% options = optimset('Display','on','Diagnostics','on','Algorithm','levenberg-marquardt','TolFun',1.0e-76,'TolCon',1.0e-76,'TolX',1.0e-76);
% fo = fminunc('FMatrixNonLinError',[F(1,:) F(2,:) F(3,:)]',options,[x1 ; x2],1.96*1.96)

F(1,1)=fo(1,1);
F(1,2)=fo(2,1);
F(1,3)=fo(3,1);
F(2,1)=fo(4,1);
F(2,2)=fo(5,1);
F(2,3)=fo(6,1);
F(3,1)=fo(7,1);
F(3,2)=fo(8,1);
F(3,3)=fo(9,1);
[U,D,V] = svd(F,0);
F = U*diag([D(1,1) D(2,2) 0])*V';
F=F/(F(3,3));


[bestInliers, bestF, residuals, meaner,varer,meder,numins] = sampsonF(T2'*F*T1, corrsclean );


display([' after nonlin error was ' num2str(meaner)]);

% Denormalise
F = T2'*F*T1;
F=F/F(3,3);

if nargout == 3  	% Solve for epipoles
    [U,D,V] = svd(F,0);
    e1 = hnormalise(V(:,3));
    e2 = hnormalise(U(:,3));
end



end



%--------------------------------------------------------------------------
% Function to check argument values and set defaults

function [x1, x2, npts,weights] = checkargs(arg);

if length(arg) == 3
    x1 = arg{1};
    x2 = arg{2};
    weights=arg{3};
    
    if ~all(size(x1)==size(x2))
        error('x1 and x2 must have the same size');
    elseif size(x1,1) ~= 3
        error('x1 and x2 must be 3xN');
    end
elseif length(arg) == 2
    if size(arg{1},1) ~= 6
        x1 = arg{1};
        x2 = arg{2};
        npts = size(x1,2);
        weights=ones(npts,1);
    elseif size(arg{1},1) == 6
        x1 = arg{1}(1:3,:);
        x2 = arg{1}(4:6,:);
        weights=arg{2};
    else
        
        error('Single argument x must be 6xN');
    end
    
elseif length(arg) == 1
    if size(arg{1},1) ~= 6
        error('Single argument x must be 6xN');
    else
        x1 = arg{1}(1:3,:);
        x2 = arg{1}(4:6,:);
       npts = size(x1,2);
        weights=ones(npts,1);
    end
else
    error('Wrong number of arguments supplied');
end

npts = size(x1,2);
if npts < 8
    error('At least 8 points are needed to compute the fundamental matrix');
end
end
