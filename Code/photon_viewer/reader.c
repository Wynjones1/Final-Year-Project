#include "reader.h"

#include <stdio.h>
#include <stdlib.h>

float *reader_points(const char *filename, int *num)
{
	FILE *fp = fopen(filename, "r");
	if(!fp)
	{
		fprintf(stderr, "%s:%d: Could not open file %s\n", __FILE__, __LINE__, filename);
		exit(-1);
	}
	if(!fscanf(fp, "%d", num) || feof(fp))
	{
		fprintf(stderr, "%s:%d Ill formed input\n", __FILE__, __LINE__);
		exit(-1);
	}
	float *out = malloc(sizeof(float) * 6 * *num);
	int i;
	printf("Reading %d points.\n", *num);
	for(i = 0; i < *num; i++)
	{
		float *o = out + i * 6;
		if(fscanf(fp, "%f %f %f %f %f %f", o, o + 1, o + 2, o + 3, o + 4, o + 5) != 6)
		{
			fprintf(stderr, "%s:%d: Ill formed input\n", __FILE__, __LINE__);
		}
		if(feof(fp))
		{
			fprintf(stderr, "%s:%d: unexpected eof\n", __FILE__, __LINE__);
		}
		for(int j = 0; j < 3; j++)
		{
			//o[j + 3] = (o[j + 3] + 3.0) / 4.0;
			o[j + 3] = 1;
		}
	}
	return out;
}
