function imOut = addSeamV(im, seam)
%removeSeamV Remove all pixels in seam

imOut = uint8(zeros(size(im,1), size(im,2) + 1, size(im, 3)));

for i = 1:size(seam, 1)
    buff = im(i,:,:);
    buff = [buff(:, 1:seam(i), :) ...
        buff(:, seam(i), :) ...
        buff(:, seam(i)+1:size(im, 2), :)];
    imOut(i,:,:) = buff;
end

end