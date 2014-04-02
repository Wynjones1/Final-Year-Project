#include "projection.h"
#include "utils.h"
#include "list.h"
#include <string.h>
#include <stdlib.h>
#include "vector.h"


projection_map_t *projection_map(const char *filename)
{
/*
	FILE *fp = OPEN(filename, "r");
	static const int buf_size = 1024;
	char buf[buf_size + 1];
	buf[buf_size] = '\0';
	fgets(buf, buf_size, fp);
	if(feof(fp)) ERROR("Unexpected end of file.\n");
	if(strcmp(buf, "ply\n") != 0) ERROR("not a ply file.\n");
	int vertex_count = -1;
	int face_count   = -1;
	while(!feof(fp))
	{
		sscanf(buf, "element vertex %d", &vertex_count);
		sscanf(buf, "element  face %d", &face_count);
		if(strcmp(buf, "end_header\n") == 0)
		{
			break;
		}
		fgets(buf, buf_size, fp);
	}
	if(feof(fp))           ERROR("Unexpected end of file\n");
	if(vertex_count == -1) ERROR("Did not read in vertex count.\n");
	if(face_count == -1)   ERROR("Did not read in face count.\n");

	list_t *vertices  = list(vertex_t);
	list_t *triangles = list(triangle_t);
	//Read In the vertex data.
	vertex_t v;
	double *d = v.data;
	fgets(buf, buf_size, fp);

	for(int i = 0; !feof(fp) && i < vertex_count; i++)
	{
		if(sscanf(buf, "%lf %lf %lf", d, d + 1, d + 2) != 3)
		{
			ERROR("Vertex Line must contain three vertices");
		}
		list_push(vertices, &v);
		fgets(buf, buf_size, fp);
	}
	//Read In the face data.
	triangle_t tri;
	int    *t = tri.vertices;
	for(int i = 0; !feof(fp) && i < vertex_count; i++)
	{
		if(sscanf(buf, "3 %d %d %d", t, t + 1, t + 2) != 3)
		{
			ERROR("Face Line must contain three vertices");
		}
		list_push(triangles, &tri);
		fgets(buf, buf_size, fp);
	}
	//Create The Map.
	projection_map_t *out = malloc(sizeof(projection_map_t));
	out->num_samples      = 0;
	out->num_tris         = face_count;
	out->last             = -1;
	out->triangles        = list_data(triangles);
	out->num_tris         = list_size(triangles);
	out->vertices         = list_data(vertices);
	free(vertices);
	free(triangles);
	return out;
*/
	return NULL;
}

double projection_map_sample(projection_map_t *map, double out[3])
{
	if(map->num_samples < 1000)
	{
	}
	return 0.0;
}

void projection_map_free(projection_map_t *map)
{
	free(map->indices);
	free(map->vertices);
	free(map->triangles);
	free(map);
}
