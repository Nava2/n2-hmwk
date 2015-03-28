swanIm = imread('Images/swan.png');

% Sobel mask
M = [ -1 0 1; 
      -2 0 2;
      -1 0 1];

swanMasked = applyMask(swanIm, M);

fprintf('absTotal = %d\n', sum(sum(abs(swanMasked))));

swanFiltered = round(stretch(swanMasked));

% save the image
imwrite(uint8(swanFiltered), 'SwanFiltered.png');
fprintf('Output saved to SwanFiltered.png\n');