all:
	gcc -o speex speex_dec.c -I/home/chris/work/ffmpeg/refs/ffmpeg_speex/include -L/home/chris/work/ffmpeg/refs/ffmpeg_speex/lib -lavformat -lavcodec -lavutil -lpthread -lm -lz -lspeex
