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

[row,col,d] = size(im);
segm = ones(row,col);  % return the whole image as the foreground 
