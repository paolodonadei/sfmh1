function [ Fgt,k1,k2,corrs, I1, I2, I1motion, R, t ] = generateCorrsforF(numcorrs, outlierratio, stderror, type )


%for synthetic data
if(type=='s')
    oldFolder = cd('../utils/CRC2D/');
    partonecom='./crc2Dto3D ';
    partTwo=[' -N ' num2str(numcorrs) ' -E ' num2str(outlierratio) ' -v ' num2str(stderror) ' -T 3 -q 1'];
    comma=[partonecom partTwo];
    [status currdir] = system(comma );
    M = dlmread('matches.csv',',',1);
    
    I1=imread('correspondences_drprevimg.png');
    I2=imread('correspondences_drnextimg.png');
    I1motion=imread('correspondences_dr.png');
    k1=load('intrinsic_synth_data.txt');
    k2=load('intrinsic_synth_data2.txt');
    Fgt=load('trueFmatrix_synth_data.txt');
    R=load('rotation_synth_data.txt');
    t=load('trans_synth_data.txt');
    x1=M(:,2:3)';
    x2=M(:,4:5)';
    status=M(:,6)';
    corrs=[x1 ; x2];
    cd(oldFolder);
end










end