#ifndef PROJECTION
#define PROJECTION

typedef struct vertex
{
	double data[3];
}vertex_t;

typedef struct triangle
{
	int vertices[3];
}triangle_t;

typedef struct projection_map
{
	int         num_samples;
	int         num_tris;
	int        *indices;
	int         last;
	vertex_t   *vertices;
	triangle_t *triangles;
}projection_map_t;

projection_map_t *projection_map(const char *filename);
void projection_map_free(projection_map_t *map);
double projection_map_sample(projection_map_t *map, double out[3]);
#endif
