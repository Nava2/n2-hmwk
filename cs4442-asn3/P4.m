%P4 

catIm = imread('Images/cat.png');
faceIm = imread('Images/face.jpg');

% [catOut, cost] = intelligentResize(catIm, -20, -20, [1 -2 1]);
% 
% imwrite(catOut, 'catResized.png');
% imtool(catOut);

E = computeEngColor(catIm, [1 1 1]);

imtool(E);

fprintf('Total cat cost: %d\n', cost);

[faceOut, cost] = intelligentResize(faceIm, -20, 10, [1 -2 1]);

imwrite(faceOut, 'faceResized.png');
imtool(faceOut);

fprintf('Total face cost: %d\n', cost);
