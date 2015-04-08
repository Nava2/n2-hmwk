function imOut = removeSeamV(im, seam)
%removeSeamV Remove all pixels in seam

imOut = uint8(zeros(size(im,1), size(im,2) - 1, size(im, 3)));

for i = 1:size(im, 1)
    buff = im(i,:,:);
    buff(:,seam(i),:) = [];
    imOut(i,:,:) = buff;
end

end

