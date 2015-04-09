% Load images
teddy_left = imread('Images/ForExtraCreditStereo/teddy-left.pgm');
teddy_right = imread('Images/ForExtraCreditStereo/teddy-right.pgm');
tsukuba_left = imread('Images/ForExtraCreditStereo/tsukuba-left.pgm');
tsukuba_right = imread('Images/ForExtraCreditStereo/tsukuba-right.pgm');
venus_left = imread('Images/ForExtraCreditStereo/venus-left.pgm');
venus_right = imread('Images/ForExtraCreditStereo/venus-right.pgm');

% for each pair, compute and display
teddy_disp = stereoCorrespondence(teddy_left, teddy_right, 7, 50);
imwrite(uint8(stretch(teddy_disp)), 'teddy_disparity.png');

venus_disp = stereoCorrespondence(venus_left, venus_right, 13, 50);
imwrite(uint8(stretch(venus_disp)), 'venus_disparity.png');

tsukuba_disp = stereoCorrespondence(tsukuba_left, tsukuba_right, 13, 50);
imwrite(uint8(stretch(tsukuba_disp)), 'tsukuba_disparity.png');

fprintf('Disparity images written to: ./NAME_disparity.png\n');