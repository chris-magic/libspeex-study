/*
 * speex_dec.c
 *
 *  Created on: Jun 18, 2013
 *      Author: chris
 */

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	AVFormatContext *pFormatCtx; //Format I/O context.
	int i, audio_stream;
	AVCodecContext *pCodecCtx;
	AVCodec *pCodec;

	AVFrame *audio_frame;
	AVPacket packet;

	if (argc < 2) {
		printf("Please provide a movie file\n");
		return -1;
	}

	FILE *pcm = fopen("tmp.pcm", "wb+");
	if (pcm == NULL) {
		printf("pcm file null ...\n");
		exit(1);
	}

	av_register_all();
	avformat_network_init();

	pFormatCtx = NULL;
	if (avformat_open_input(&pFormatCtx, argv[1], NULL, NULL) != 0) {

		printf("av_open_input_file failed \n");
		exit(1);
	}

	if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
		printf("Couldn't find stream information\n");
		exit(1);
	}

	av_dump_format(pFormatCtx, 0, argv[1], 0);

	audio_stream = -1;
	for (i = 0; i < pFormatCtx->nb_streams; i++) {
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
			audio_stream = i;
			break;
		}
	}
	if (audio_stream == -1) {
		printf("Didn't find a video stream \n");
		exit(1);
	}

	pCodecCtx = pFormatCtx->streams[audio_stream]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL) {
		fprintf(stderr, "Unsupported codec!\n");
		exit(1); // Codec not found
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
		printf("Could not open codec \n");
		exit(1);
	}
	audio_frame = avcodec_alloc_frame();
	if (audio_frame == NULL) {
		printf("audio_frame allocate failed\n");
		exit(1);
	}

	i = 0;
	int got_frame_ptr = 0;
	while (av_read_frame(pFormatCtx, &packet) >= 0) {
		if (packet.stream_index == audio_stream) {
			uint8_t *tmp_ptr = packet.data;
			while (packet.size > 0) {
				int len = avcodec_decode_audio4(pCodecCtx, audio_frame,
						&got_frame_ptr, &packet);
				if (len < 0) //解码失败
				{
					printf("Error while decoding.\n");
					break;
				}

				if (got_frame_ptr) {
					/* if a frame has been decoded, output it */
					int data_size = av_samples_get_buffer_size(NULL,
							pCodecCtx->channels, audio_frame->nb_samples,
							pCodecCtx->sample_fmt, 1);
					fwrite(audio_frame->data[0], 1, data_size, pcm);
				}

				packet.size -= len; //计算packet中还剩下多少
				packet.data += len; //计算packet的位移,注意我这里对packet.data进行了修改,所以造成最后释放av_free_packet会有问题,因为av_free_packet最后调用的就是释放packet.data指向的数据.
			} //end while
			packet.data = tmp_ptr;
		}

		av_free_packet(&packet);
	}

	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);

	return 0;
}

