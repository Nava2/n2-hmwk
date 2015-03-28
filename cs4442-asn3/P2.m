% P2

faceIm = imread('Images/face.jpg');

E = computeEngGradH(faceIm);

fprintf('Energy total: %d\n', sum(sum(E)));

imwrite(uint8(stretch(E)), 'faceEngG.jpg');
fprintf('Output saved to: faceEngG.jpg\n');
