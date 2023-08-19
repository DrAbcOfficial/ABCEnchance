Create your movie using:

ffmpeg -i .\notporno.ivf -vf scale=1280:-1 -c:v libvpx-vp9 -f ivf -pix_fmt yuv444p -r 30 notporno.ivf

do not use very high resolution or frame rate, or perform will be super duper bad.
30fps 1280x714 are fine.
vp9 and vp8 are fine.