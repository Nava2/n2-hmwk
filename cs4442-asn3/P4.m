%P4 

catIm = imread('Images/cat.png');
[catOut, cost] = intelligentResize(catIm, -20, -20, [1 -2 1]);
imwrite(catOut, 'catResized.png');

fprintf('Total cat cost: %d\n', cost);

% run for faces
faceIm = imread('Images/face.jpg');
[faceOut, cost] = intelligentResize(faceIm, -20, 10, [1 -2 1]);
imwrite(faceOut, 'faceResized.jpg');

fprintf('Total face cost: %d\n', cost);

% run for pigs
pigsIm = imread('Images/pigs.png');
[pigsOut, cost] = intelligentResize(pigsIm, 20, -10, [1 -2 1]);
imwrite(pigsOut, 'pigsResized.png');

fprintf('Total pigs cost: %d\n', cost);
