#ifndef UTILS_H
#define UTILS_H
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

void   fprint_bin(FILE* fp , uint64_t in);
void   print_bin(uint64_t in);
int    contains(double min[3], double max[3], double point[3]);
void   dswap(double *a,double *b);
uint32_t convert_double_rgb_to_24bit(double *colour);
void progress_bar(int done, int out_of, int width);

#define BUFCMP(buffer, string) strncmp(buffer, string, strlen(string))
#define STRINGIFY(X) #X
#define TOSTRING(X) STRINGIFY(X)
#if NDEBUG
	#define Assert(error, message) do{(void)(error);}while(0);
#else
	#define Assert(error, message) (assert(error && message))
#endif
#define NotImplemented() Assert(0, "Not Implemented")

void print_warning(const char *format, const char *file, int line, ...);
void print_error(const char *format, const char *file, int line, ...) __attribute__((noreturn));
void print_output(const char *format, ...);
void print_verbose(const char *format, ...);
void fprint_output(FILE *fp, const char *format, ...);
FILE *open_file(const char *filename, const char *mode, const char *file, int line);

#define WARNING(format, ...) print_warning(format, __FILE__, __LINE__, ##__VA_ARGS__)
#define ERROR(format, ...)   print_error(format, __FILE__, __LINE__, ##__VA_ARGS__)
#define OUTPUT(...)          print_output(__VA_ARGS__)
#define FOUTPUT(fp, ...)    fprint_output(fp, ##__VA_ARGS__)
#define VERBOSE(format, ...) print_verbose(format, ##__VA_ARGS__);
#define OPEN(filename, mode) open_file(filename, mode, __FILE__, __LINE__);

#endif
