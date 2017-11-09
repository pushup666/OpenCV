data1 = load('D:\Codecs\data - TEST5K_ffmpeg.txt');
psnr_data1 = data1(:,1);
ssim_data1 = data1(:,2);
data2 = load('D:\Codecs\data - TEST5K_x264.txt');
psnr_data2 = data2(:,1);
ssim_data2 = data2(:,2);

smooth_psnr_data1 = smooth(psnr_data1);
smooth_psnr_data2 = smooth(psnr_data2);
smooth_ssim_data1 = smooth(ssim_data1);
smooth_ssim_data2 = smooth(ssim_data2);

figure(1);
plot(smooth_psnr_data1,'r');
hold on;
plot(smooth_psnr_data2,'b');
title('psnr');

figure(2);
plot(smooth_ssim_data1,'r');
hold on;
plot(smooth_ssim_data2,'b');
title('ssim');

mean_psnr_data1 = mean(psnr_data1);
mean_psnr_data2 = mean(psnr_data2);
mean_ssim_data1 = mean(ssim_data1);
mean_ssim_data2 = mean(ssim_data2);

