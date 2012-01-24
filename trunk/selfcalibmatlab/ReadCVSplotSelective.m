function []=ReadCVSplotSelective(filename)

% skipping the header
M = csvread(filename,1,0);

%get the number of columns
n=size(M,2);

% read the header
fid=fopen(filename);
C_text=textscan(fid, '%s', n, 'delimiter', ',');
fclose(fid);

if(strcmp(C_text{1}(n),'0')~=0)
    n=n-1;
end

end