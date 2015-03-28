function [ outIm ] = applyMask( im, M )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

mSzD2 = floor(size(M) / 2);

imSz = size(im);
buff = zeros(imSz(1) + 2*mSzD2(1), imSz(2) + 2*mSzD2(2));
buff(1 + mSzD2(1):imSz(1) + mSzD2(1), 1 + mSzD2(2):imSz(2) + mSzD2(2)) = im;

outIm = zeros(imSz(1) + 2*mSzD2(1), imSz(2) + 2*mSzD2(2));

for i = 1 + mSzD2(1):imSz(1) + mSzD2(1)
    for j = 1 + mSzD2(2):imSz(2) + mSzD2(2)
       outIm(i, j) = sum(sum(M .* buff(i-mSzD2(1):i+mSzD2(1), j-mSzD2(2):j+mSzD2(2)), 1));
    end
end

outIm = outIm(1+mSzD2(1):mSzD2(1)+imSz(1), 1+mSzD2(2):mSzD2(2)+imSz(2));

end

