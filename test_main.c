/*
 * test_main.c
 *
 *  Created on: Jun 19, 2013
 *      Author: chris
 */

#include <stdio.h>

#include "speex_encode.h"

int main(int argc ,char * argv[]){

	FILE *fin ,*fout;
	//fin = fopen(argv[1], "r");
	fin = fopen(argv[1], "rb"); //use rb
	fout = fopen(argv[2] ,"wb+"); //use wb+ ,can not use w only
	short in[160];

	char speex_data[38];

	int handle = spx_encode_init();

	while (1) {
			/*Read a 16 bits/sample audio frame*/
			fread(in, sizeof(short), 160, fin);
			if (feof(fin))
				break;

			spx_encode_frame(handle ,in ,speex_data);

			fwrite(speex_data ,38 ,1 ,fout);
	}
	fclose(fout);

}


