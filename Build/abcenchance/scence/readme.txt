Create your movie using:

ffmpeg -i .\whatever.mp4 -c:v libvpx-vp9 -f ivf -pix_fmt yuv444p whatever.ivf

vp9 and vp8 are fine.