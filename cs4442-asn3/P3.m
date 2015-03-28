% P3

catIm = imread('Images/cat.png');

w = [1 -2 1];
E = computeEngColor(catIm, w);

fprintf('Energy total: %d\n', sum(sum(E)));

imwrite(uint8(stretch(E)), 'catEngC.png');
fprintf('Output saved to: catEngC.png\n');
