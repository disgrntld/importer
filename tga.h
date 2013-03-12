typedef struct
{
	unsigned char depth;
	short int w, h;
	unsigned char* data;
} tga_data_t;

tga_data_t* tga_data_load(char* fn);

