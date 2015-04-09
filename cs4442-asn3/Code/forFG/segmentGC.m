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

VARIANCE = 2 ^ 2;
INFINITY_VALUE = 5000;

    function w = calcW(p, q)
        d = sum((p - q) .^ 2);
        w = 20*exp(-1 * d / (2 * VARIANCE));
    end

E = computeEngColor(im, [1 1 1]);

M = E(scribbleMask == 2);
fmin = min(M);
fmax = max(M);

Df = fmax - E;
Db = E - fmin;

Df(scribbleMask == 2) = INFINITY_VALUE; % set to "infinity"
Db(scribbleMask == 2) = 0;

Df(scribbleMask == 1) = 0;
Db(scribbleMask == 1) = INFINITY_VALUE; % set to "infinity"

dataF = reshape(Df, 1, size(E, 1) * size(E, 2));
dataB = reshape(Db, 1, size(E, 1) * size(E, 2));

w = size(Df, 1);
h = size(Df, 2);

W = zeros(2*(2 * w * h - w - h), 3); % preallocate, it's huge

% rows
rInd = 1;
for j = 1:size(im, 1)
    for i = 1:size(im, 2)-1
        w = calcW(im(j, i, :), im(j, i+1,:));
        idx1 = sub2ind(size(im), j, i);
        idx2 = sub2ind(size(im), j, i+1);
        W(rInd, :) = [idx1 idx2 w];
        W(rInd+1, :) = [idx2 idx1 w];
        
        rInd = rInd + 2;
    end
end

% columns
for j = 1:size(im, 1)-1
    for i = 1:size(im, 2)
        w = calcW(im(j, i, :), im(j+1, i,:));
        idx1 = sub2ind(size(im), j, i);
        idx2 = sub2ind(size(im), j+1, i);
        W(rInd, :) = [idx1 idx2 w];
        W(rInd+1, :) = [idx2 idx1 w];
        
        rInd = rInd + 2;
    end
end

labels = solveMinCut(dataB, dataF, W);
segm = reshape(labels, size(E));


end
