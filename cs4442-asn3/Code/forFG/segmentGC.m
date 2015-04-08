%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Performs foreground/background segmentation based on a graph cut
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% INPUT: image im and scribbleMask with scribbles
% scribbleMask(i,j) = 2 means pixel(i,j) is a foreground seed
% scribbleMask(i,j) = 1 means pixel(i,j) is a background seed
% scribbleMask(i,j) = 0 means pixel(i,j) is not a seed
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% OUTPUT: segm is the segmentation mask of the  same size as input image im
% segm(i,j) = 1 means pixel (i,j) is the foreground
% segm(i,j) = 0 means pixel (i,j) is the background
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function segm  = segmentGC(im,scribbleMask)

E = computeEngColor(im, [1 1 1]);

M = im;
[I, J] = ind2sub(size(E), find(E < 175));
for i_ = 1:size(I)
    M(I(i_), J(i_), :) = 0;
end
imtool(uint8(M));

M = im;
[I, J] = ind2sub(size(E), find(E < 220));
for i_ = 1:size(I)
    M(I(i_), J(i_), :) = 0;
end
imtool(uint8(M));

M = E(scribbleMask == 2);
fmin = min(M);
fmax = max(M);

Df = fmax - E;
Db = E - fmin;

[row,col,d] = size(im);
segm = ones(row,col);  % return the whole image as the foreground 


end
