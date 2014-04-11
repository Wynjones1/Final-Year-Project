#include "utils.h"
#include <stdio.h>
#include "vector.h"
#include "ray.h"
#include "maths.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <errno.h>

void print_bin(uint64_t in)
{
	fprint_bin(stdout, in);
}

void fprint_bin(FILE* fp , uint64_t in)
{
	int count = 0;
	for(int i = 63; i >= 0; i--)
	{
		if(count != 0 && !(count % 4))
		{
			fprintf(fp, " ");
		}
		if( (in >> i) & 0x1)
		{
			fprintf(fp, "1");
		}
		else
		{
			fprintf(fp, "0");
		}
		count++;
	}
}


int contains(double min[3], double max[3], double point[3])
{
	for(int i = 0; i < 3; i++)
	{
		if(!(min[i] < point[i] && point[i] < max[i]))
			return 0;
	}
	return 1;
}

void dswap(double *a,double *b)
{
	double temp = *a;
	*a = *b;
	*b = temp;
}

void print_warning(const char *format, const char *file, int line, ...)
{
	char buf[1000];
	sprintf(buf, "%s:%d: WARN %s", file, line, format);
	va_list args;
	va_start(args, line);
	vprintf(buf, args);
	va_end(args);
}

void print_error(const char *format, const char *file, int line, ...)
{
	char buf[1000];
	sprintf(buf, "%s:%d: ERROR: %s", file, line, format);
	va_list args;
	va_start(args, line);
	vprintf(buf, args);
	va_end(args);
	abort();
}

void fprint_output(FILE *fp, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(fp, format, args);
	va_end(args);
}

void print_output(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}

void print_verbose(const char *format, ...)
{
#if VERBOSE_OUTPUT
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
#endif
}

FILE *open_file(const char *filename, const char *mode, const char *file, int line)
{
	errno = 0;
	FILE *fp = fopen(filename, mode);
	if(!fp)
	{
		print_error("Could not open file %s: %s\n", file, line,filename, strerror(errno));
	}
	return fp;
}

double clamp(double in)
{
	return in < 0 ? 0 : in > 1 ? 1 : in;
}

uint32_t convert_double_rgb_to_24bit(double *colour)
{
	uint32_t out = 0;
	#if 0
	double norm = max(1.0, max(colour[0], max(colour[1], colour[2])));
	out |= (uint8_t)(max(0, min(255, (255 * colour[0] / norm)))) << 16;
	out |= (uint8_t)(max(0, min(255, (255 * colour[1] / norm)))) << 8;
	out |= (uint8_t)(max(0, min(255, (255 * colour[2] / norm))));
	#else
	out |= (int)(pow(clamp(colour[0]), 1/2.2) * 255 + 0.5) << 16;
	out |= (int)(pow(clamp(colour[1]), 1/2.2) * 255 + 0.5) << 8;
	out |= (int)(pow(clamp(colour[2]), 1/2.2) * 255 + 0.5) << 0;
	#endif
	return out;
}

void progress_bar(int done, int out_of, int width)
{
#if VERBOSE_OUTPUT
	if(done <= out_of)
	{
		double percent = ((double)done) / out_of;
		int fin = (int) (width * percent);
		VERBOSE("[");
		for(int i = 0; i < fin; i++)VERBOSE("#");
		for(int i = 0; i < width - fin; i++)VERBOSE(".");
		VERBOSE("] %6.2f%%\r", 100 * percent)
		if(done == out_of)
		{
			VERBOSE("\n");
		}
		if(done == 0)
		{
			fflush(stdout);
		}
	}
#endif
}
