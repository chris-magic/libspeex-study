/*
 * sampleenc.c
 *
 *  Created on: Jun 18, 2013
 *      Author: chris
 */

#include <speex/speex.h>
#include <stdio.h>
#include <string.h>
/*The frame size in hardcoded for this sample code but it doesn’t have to be*/
#define FRAME_SIZE 160
int main(int argc, char **argv) {
	char *inFile;
	FILE *fin;
	short in[FRAME_SIZE];
	float input[FRAME_SIZE];
	char cbits[200];
	int nbBytes;
	/*Holds the state of the encoder*/
	void *state;
	/*Holds bits so they can be read and written to by the Speex routines*/
	SpeexBits bits;
	int i, tmp;

	/*Create a new encoder state in narrowband mode*/
	state = speex_encoder_init(&speex_nb_mode);

	/*Set the quality to 8 (15 kbps)*/
	tmp = 8;
	speex_encoder_ctl(state, SPEEX_SET_QUALITY, &tmp);

	inFile = argv[1];
	fin = fopen(inFile, "rb");

	FILE *fout = fopen(argv[2] ,"wb+");
	/*Initialization of the structure that holds the bits*/
	speex_bits_init(&bits);
	while (1) {
		/*Read a 16 bits/sample audio frame*/
		fread(in, sizeof(short), FRAME_SIZE, fin);
		if (feof(fin))
			break;
		/*Copy the 16 bits values to float so Speex can work on them*/
		for (i = 0; i < FRAME_SIZE; i++)
			input[i] = in[i];

//		 memcpy((char*)input,(char *)in,FRAME_SIZE*2);
		/*Flush all the bits in the struct so we can encode a new frame*/
		speex_bits_reset(&bits);

		/*Encode the frame*/
		speex_encode(state, input, &bits);
		/*Copy the bits to an array of char that can be written*/
		nbBytes = speex_bits_write(&bits, cbits, 200);
		printf("nbBytes = %d \n" ,nbBytes);
		/*Write the size of the frame first. This is what sampledec expects but
		 it’s likely to be different in your own application*/
		fwrite(cbits, 1, nbBytes, fout);

//		fwrite(&nbBytes, sizeof(int), 1, stdout);
//		printf("nbBytes = %d \n" ,nbBytes);
//		/*Write the compressed data*/
		//fwrite(cbits, 1, nbBytes, stdout);

	}
	fclose(fout);

	/*Destroy the encoder state*/
	speex_encoder_destroy(state);
	/*Destroy the bit-packing struct*/
	speex_bits_destroy(&bits);
	fclose(fin);
	return 0;
}

