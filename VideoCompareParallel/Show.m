data1 = load('D:\Codecs\Beauty.libx264.5k.txt');
psnr_data1 = data1(:,1);
ssim_data1 = data1(:,2);
data2 = load('D:\Codecs\Beauty.libx265.5k.crf28.txt');
psnr_data2 = data2(:,1);
ssim_data2 = data2(:,2);
data3 = load('D:\Codecs\Beauty.libx265.5k.crf28.txt');
psnr_data3 = data3(:,1);
ssim_data3 = data3(:,2);

smooth_psnr_data1 = smooth(psnr_data1);
smooth_ssim_data1 = smooth(ssim_data1);
smooth_psnr_data2 = smooth(psnr_data2);
smooth_ssim_data2 = smooth(ssim_data2);
smooth_psnr_data3 = smooth(psnr_data3);
smooth_ssim_data3 = smooth(ssim_data3);

figure(1);hold on;
plot(smooth_psnr_data1,'r');
plot(smooth_psnr_data2,'g');
plot(smooth_psnr_data3,'b');
title('psnr');

figure(2);hold on;
plot(smooth_ssim_data1,'r');
plot(smooth_ssim_data2,'g');
plot(smooth_ssim_data3,'b');
title('ssim');

mean_psnr_data1 = mean(psnr_data1);
mean_ssim_data1 = mean(ssim_data1);
mean_psnr_data2 = mean(psnr_data2);
mean_ssim_data2 = mean(ssim_data2);
mean_psnr_data3 = mean(psnr_data3);
mean_ssim_data3 = mean(ssim_data3);
