/*
 * speex_enc.c
 *
 *  Created on: Jun 18, 2013
 *      Author: chris
 */
#include <stdio.h>

#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/mathematics.h"
#include "libavutil/samplefmt.h"

int main(int argc, char* argv[]) {

	AVCodec *ptr_codec;
	AVCodecContext *ptr_codec_context = NULL;
	int frame_size;
	int out_size, outbuf_size;
	FILE *f;
	uint8_t *samples;
	uint8_t *outbuf;

	/*	register codecs and formats*/
	av_register_all();

	printf("Audio encoding\n");

	/* find the aac encoder */
//	ptr_codec = avcodec_find_encoder(CODEC_ID_AAC);
	ptr_codec = avcodec_find_encoder(AV_CODEC_ID_SPEEX); //AV_CODEC_ID_SPEEX
	if (!ptr_codec) {
		fprintf(stderr, "codec not found\n");
		exit(1);
	}

	ptr_codec_context = avcodec_alloc_context3(ptr_codec);

	/* put sample parameters */
	//libspeexenc.c
//	ptr_codec_context->bit_rate = 56000; //the average bitrate
	ptr_codec_context->sample_rate = 8000; // Speex has 3 modes, each of which uses a specific sample rate
	ptr_codec_context->channels = 1; //number of audio channels
	ptr_codec_context->sample_fmt = AV_SAMPLE_FMT_S16; //sample format
//	ptr_codec_context->global_quality = 2;

	AVDictionary *opts = NULL;
	av_dict_set(&opts, "cbr_quality", "8", 0);


	/* open it */
	if (avcodec_open2(ptr_codec_context, ptr_codec ,&opts) < 0) {
		fprintf(stderr, "could not open codec\n");
		exit(1);
	}

	/* the codec gives us the frame size, in samples */
	frame_size = ptr_codec_context->frame_size;
	printf("c->frame_size = %d ...\n", ptr_codec_context->frame_size);
	samples = malloc(frame_size * 2 * ptr_codec_context->channels);
	printf("samples = %d \n", frame_size * 2 * ptr_codec_context->channels);
	//这里的数值是固定的？2是这样来的
	//av_get_bytes_per_sample(ptr_codec_context->sample_fmt) 这里获取的值就是2
	//这里是参考ffmpeg.c中这个函数的使用
	printf("ptr_codec_context->channels = %d ...\n",
			ptr_codec_context->channels);
	printf("samples = %d ...\n", frame_size * 2 * ptr_codec_context->channels);

	outbuf_size = 10000;
	outbuf = malloc(outbuf_size);

	f = fopen(argv[1], "wb");
	if (!f) {
		fprintf(stderr, "could not open %s\n", argv[1]);
		exit(1);
	}

	FILE *p_source = fopen(argv[2], "r+");

	printf("before while ....\n");
	while (!feof(p_source)) {
		fread(samples, 1, frame_size * 2 * ptr_codec_context->channels,
				p_source);

		//		/* encode the samples */
		//out packet
		AVPacket pkt;
		av_init_packet(&pkt);
		pkt.data = NULL;
		pkt.size = 0;

		//in frame
		AVFrame *frame = avcodec_alloc_frame();
		if (frame == NULL) {
			printf("frame malloc failed ...\n");
			exit(1);
		}
		avcodec_get_frame_defaults(frame);
		frame->nb_samples = (frame_size * 2 * ptr_codec_context->channels) / 2;   //number of audio samples (per channel) described by this frame

	/*说明一点frame->nb_samples 这个一定要设置正确哈。。*/
		int ret;
		if ((ret = avcodec_fill_audio_frame(frame, ptr_codec_context->channels/*2*/, AV_SAMPLE_FMT_S16,
				samples, (frame_size * 2 * ptr_codec_context->channels), 1))
				< 0) {
			av_log(NULL, AV_LOG_FATAL, "Audio encoding failed...\n");
			exit(1);
		}

		int got_packet = 0;
		if (avcodec_encode_audio2(ptr_codec_context, &pkt, frame, &got_packet)
				< 0) {
			av_log(NULL, AV_LOG_FATAL, "Audio encoding failed\n");
			exit(1);
		}

		if (got_packet) {
			printf("write data ..pkt.size = %d.\n" ,pkt.size);
			fwrite(pkt.data, 1, pkt.size, f);
		}
		av_free_packet(&pkt);
		av_free(frame);
	}

	fclose(f);
	free(outbuf);
	free(samples);

	avcodec_close(ptr_codec_context);
	av_free(ptr_codec_context);

	/*free sth*/

	return 0;
}





