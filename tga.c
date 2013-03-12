#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "tga.h"

#define uchar unsigned char
#define sint short int

const size_t size_uchar = sizeof(uchar);
const size_t size_sint = sizeof(sint);

tga_data_t* tga_data_load(char* fn)
{
	tga_data_t* tga = NULL;
	FILE* fh = NULL;
	int md, t;

	/* Allocate memory for the info structure. */
	tga = malloc(sizeof(tga_data_t));

	/* Open the file in binary mode. */
	fh = fopen(fn, "rb");

	/* Problem opening file? */
	if (fh == NULL)
	{
		fprintf(stderr, "Error: problem opening TGA file (%s).\n", fn);
	}

	else
	{
		tga = malloc(sizeof(tga_data_t));

		// Load information about the tga, aka the header.
		{
			// Seek to the width.
			fseek(fh, 12, SEEK_SET);
			fread(&tga->w, size_sint, 1, fh);
			
			// Seek to the height.
			fseek(fh, 14, SEEK_SET);
			fread(&tga->h, size_sint, 1, fh);
			
			// Seek to the depth.
			fseek(fh, 16, SEEK_SET);
			fread(&tga->depth, size_sint, 1, fh);
		}
		
		// Load the actual image data.
		{
			// Mode = components per pixel.
			md = tga->depth / 8;

			// Total bytes = h * w * md.
			t = tga->h * tga->w * md;

			printf("Reading %d bytes.\n", t);

			// Allocate memory for the image data.
			tga->data = malloc(size_uchar * t);

			// Seek to the image data.
			fseek(fh, 18, SEEK_SET);
			fread(tga->data, size_uchar, t, fh);

			// We're done reading.
			fclose(fh);

			// Mode 3 = RGB, Mode 4 = RGBA
			// TGA stores RGB(A) as BGR(A) so
			// we need to swap red and blue.
			if (md >= 3)
			{
				uchar aux;

				int i;
				for (i = 0; i < t; i+= md)
				{
					aux = tga->data[i];
					tga->data[i] = tga->data[i + 2];
					tga->data[i + 2] = aux;
				}
			}
		}

		printf("Loaded texture -> (%s)\nWidth: %d\nHeight: %d\nDepth: %d\n", fn, tga->w, tga->h, tga->depth);
	}

	return tga;
}

