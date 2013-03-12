#include <stdio.h>
#include <stdlib.h>
#include "scoReader.h"
#include "scoWriter.h"
#include "tga.h"

void set_layer(ground_paint_t *ground_paint, int layer, tga_data_t *map)
{
	int max = ground_paint->size_x * ground_paint->size_y;

	if(ground_paint->layers[layer].cells == NULL) {
		ground_paint->layers[layer].continuity_count = malloc((max + 1) * sizeof(int));
		ground_paint->layers[layer].cells = malloc(max * sizeof(float));
	}

	ground_paint->layers[layer].continuity_count[0] = max;
	ground_paint->layers[layer].continuity_count[max] = 0;

	int x, y;
	for(y = 0; y < ground_paint->size_y; ++y)
	{
		for(x = 0; x < ground_paint->size_x; ++x)
		{
			int offset = (y * map->w + x) * map->depth / 8;
			short int r = map->data[offset];
			short int g = map->data[offset + 1];
			short int b = map->data[offset + 2];
			if(r != g && g != b) {
				printf("ERROR: map was not grayscale\n");
				return;
			}
			float result = r / 255.0f;
			ground_paint->layers[layer].cells[x * ground_paint->size_y + y] = r / 255.0f;
		}
	}
}

int main(int argc, char **argv)
{
	if(argc < 12)
	{
		printf("Usage: %s input heightmap output\n", argv[0]);
		return EXIT_FAILURE;
	}

	FILE *in = fopen(argv[1], "rb");
	tga_data_t *height_map = tga_data_load(argv[2]);
	tga_data_t *gray_stone_map = tga_data_load(argv[3]);
	tga_data_t *turf_map = tga_data_load(argv[4]);
	tga_data_t *steppe_map = tga_data_load(argv[5]);
	tga_data_t *earth_map = tga_data_load(argv[6]);
	tga_data_t *desert_map = tga_data_load(argv[7]);
	tga_data_t *forest_map = tga_data_load(argv[8]);
	tga_data_t *village_map = tga_data_load(argv[9]);
	tga_data_t *path_map = tga_data_load(argv[10]);
	FILE *out = fopen(argv[11], "wb");

	if(!in || !height_map || !gray_stone_map || !turf_map || !steppe_map || !earth_map || !desert_map || !forest_map || !village_map || !path_map || !out)
	{
		printf("ERROR: file(s) not found\n");
		return EXIT_FAILURE;
	}

	printf("Reading %s\n", argv[1]);
	sco_file_t sco_file;
	read_sco_file(in, &sco_file);

	int lcv;
	for(lcv = 0; lcv < sco_file.ground_paint->num_layers; ++lcv)
	{
		printf("no: %u\n", sco_file.ground_paint->layers[lcv].ground_spec_no);
		switch(sco_file.ground_paint->layers[lcv].ground_spec_no)
		{
			case 0: // gray_stone
				set_layer(sco_file.ground_paint, lcv, gray_stone_map);
				break;
			case 2: // turf
				set_layer(sco_file.ground_paint, lcv, turf_map);
				break;
			case 3: // steppe
				set_layer(sco_file.ground_paint, lcv, steppe_map);
				break;
			case 5: // earth
				set_layer(sco_file.ground_paint, lcv, earth_map);
				break;
			case 6: // desert
				set_layer(sco_file.ground_paint, lcv, desert_map);
				break;
			case 7: // forest
				set_layer(sco_file.ground_paint, lcv, forest_map);
				break;
			case 9: // village
				set_layer(sco_file.ground_paint, lcv, village_map);
				break;
			case 10: // path
				set_layer(sco_file.ground_paint, lcv, path_map);
				break;
			case GROUND_PAINT_ELEVATION_MAGIC:
			{
				printf("Updating terrain\n");
				int x, y;
				for(y = 0; y < sco_file.ground_paint->size_y; ++y)
				{
					for(x = 0; x < sco_file.ground_paint->size_x; ++x)
					{
						int offset = (y * height_map->w + x) * height_map->depth / 8;
						short int r = height_map->data[offset];
						short int g = height_map->data[offset + 1];
						short int b = height_map->data[offset + 2];
						if(r != g && g != b)
							return EXIT_FAILURE;

						sco_file.ground_paint->layers[lcv].cells[x * sco_file.ground_paint->size_y + y] += (100.0f * 886.0f / 2625.0f) * r / 255.0f - 1.0f;
					}
				}
				break;
			}
		}
	}

	printf("Writing %s\n", argv[11]);
	write_sco_file(out, &sco_file);

	return EXIT_SUCCESS;
}

