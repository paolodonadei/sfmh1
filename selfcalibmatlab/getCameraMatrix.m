% getCameraMatrix - Given an essential matrix, compute the second camera
% matrix
%
%
% Given the essential matrix, it is decomposed and 4 possible camera
% matrices are calculated for the second camera.
%
%
% Input  - E   -> 3x3 essential matrix
%
% Output - P   -> 3x4x4 Camera matrices (rotation and translation)
%
%
%
% Author: Isaac Esteban
% IAS, University of Amsterdam
% TNO Defense, Security and Safety
% iesteban@science.uva.nl
% isaac.esteban@tno.nl

function [PXcam] = getCameraMatrix(E)


    % SVD of E
    [U,S,V] = svd(E);
    
    % Matrix W
    W = [0,-1,0;1,0,0;0,0,1];
        
    % Compute 4 possible solutions (p259)
    PXcam = zeros(3,4,4);
    PXcam(:,:,1) = [U*W*V',U(:,3)./max(abs(U(:,3)))];
    PXcam(:,:,2) = [U*W*V',-U(:,3)./max(abs(U(:,3)))];
    PXcam(:,:,3) = [U*W'*V',U(:,3)./max(abs(U(:,3)))];
    PXcam(:,:,4) = [U*W'*V',-U(:,3)./max(abs(U(:,3)))];
   
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    