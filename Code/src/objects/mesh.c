#include "vector.h"
#include "kdtree.h"
#include "utils.h"
#include "intersection.h"
#include "texture.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <stdbool.h>

static void triangle_print(mesh_t *mesh, int triangle)
{
	OUTPUT("%08d: ", triangle);
	for(int i = 0; i < 3; i++)
	{
		double *v = MESH_VERTEX(mesh, triangle, i);
		OUTPUT("(");
		for(int j = 0; j < 3; j++)
		{
			OUTPUT("%4.2f ", v[j]);
		}
		OUTPUT(") ");
	}
	OUTPUT("\n");
}

void mesh_print(mesh_t *mesh)
{
	for(int i = 0; i < mesh->triangle_count; i++)
	{
		triangle_print(mesh, i);
	}
	OUTPUT("\n");
}

/*
*	Move the pointers to each vertices of triangle i into triangle.
*/
void mesh_get_triangle(mesh_t *mesh, int i, double **triangle)
{
		triangle[0] = MESH_VERTEX(mesh, i, 0);
		triangle[1] = MESH_VERTEX(mesh, i, 1);
		triangle[2] = MESH_VERTEX(mesh, i, 2);
}

void mesh_get_triangle_indices(mesh_t *mesh, int index, int *triangle)
{
	for(int i = 0; i < 3; i++)
	{
		triangle[i] = mesh->triangles[index * 3 + i];
	}
}

static const int buffer_size          = 1024;
static const int number_of_components = 3;

int readline(FILE *fp,char *buffer)
{
	errno = 0;
	int eof = 0;
	if(fgets(buffer, buffer_size, fp) == NULL)
	{
		if(errno)
		{
			ERROR("Read failure %s\n",strerror(errno));	
		}
	}
	if(feof(fp))
	{
		eof = 1;	
	}
	return !eof;
}

static void read_ply(const char *filename, mesh_t *mesh)
{
	errno = 0;
	char    buffer[buffer_size];
	int     i,j,new_face_count = 0;
	int     index_buffer[4];
	FILE   *fp = OPEN(filename,"r");
	char   *startpoint     = NULL;
	double *current_vertex = NULL;
	double *current_normal = NULL;
	double *current_tex    = NULL;
	int    *current_index  = NULL;

	//Read in the header
	int vertex_count = 0,face_count = 0;
	bool has_vertex = false;
	bool has_normals = false;
	bool has_tex = false;
	char property_buf[2];

	mesh->normals = NULL;
	mesh->tex     = NULL;

	while(readline(fp,buffer))
	{
		if(strncmp("end_header",buffer,strlen("end_header")) == 0)
		{
			break;
		}
		sscanf(buffer,"element vertex %d",&vertex_count);	
		sscanf(buffer,"element face %d",&face_count);	
		if(sscanf(buffer, "property float %s", property_buf) == 1)
		{
			if(BUFCMP(property_buf, "x") || BUFCMP(property_buf, "y") || BUFCMP(property_buf, "z"))
			{
				has_vertex = true;
			}
			if(BUFCMP(property_buf, "nx") || BUFCMP(property_buf, "ny") || BUFCMP(property_buf, "nz"))
			{
				has_normals = true;
			}
			if(BUFCMP(property_buf, "s") || BUFCMP(property_buf, "t"))
			{
				has_tex = true;
			}
		}
	}
	Assert(has_vertex, "Did not read any vertices.\n");
	if(vertex_count)
	{
		mesh->vertices    = malloc(sizeof(double) * number_of_components * vertex_count);
		if(has_normals)
			mesh->normals = malloc(sizeof(double) * number_of_components * vertex_count);
		if(has_tex)
			mesh->tex     = malloc(sizeof(double) * 2 * vertex_count);
	}
	else
	{
		ERROR("Vertex count not in ply file %s\n", filename);
	}
	//malloc more than enough index data, it will be resized at the end.
	mesh->triangles  = malloc(sizeof(int) * face_count * 6);
	current_vertex = (double*) mesh->vertices;
	current_normal = (double*) mesh->normals;
	current_tex    = (double*) mesh->tex;
	current_index  = (int*)    mesh->triangles;
	//read in the vertex data.
	for(i = 0;i < vertex_count;i++)
	{
		readline(fp,buffer);
		startpoint = buffer;
		//Process the Vertex Data
		for(j = 0; j < number_of_components; j++)
		{
			*(current_vertex++) = strtod(startpoint,&startpoint);
		}
		if(has_normals)
		{
			for(j = 0; j < number_of_components; j++)
			{
				*(current_normal++) = strtod(startpoint,&startpoint);
			}
		}
		if(has_tex)
		{
			for(j = 0; j < 2; j++)
			{
				*(current_tex++) = strtod(startpoint,&startpoint);
			}
		}
	}
	//read in the face data.
	for(i = 0;i < face_count;i++)
	{
		readline(fp, buffer);
		startpoint = buffer;
		//get the size of the face.
		int n = strtol(startpoint,&startpoint,10);
		if(n != 3 && n != 4)
		{
			ERROR("Only tri or quads in face data is supported.\n");
		}
		for(j = 0; j < n; j++)
		{
			//get each index of the face.
			index_buffer[j] = 
				strtol(startpoint,&startpoint,10);
		}
		//store the face data and split quads if necessary
		for(j = 0; j < 3; j++)
			*(current_index++) = index_buffer[j];
		new_face_count += 1;
		if(n == 4)//add the extra face to the face count;
		{
			*(current_index++) = index_buffer[0];
			*(current_index++) = index_buffer[2];
			*(current_index++) = index_buffer[3];
			new_face_count += 1;
		}
	}
	//Resize the indices to the correct size now that we know it.
	if(new_face_count)
	{
		mesh->triangles = realloc(mesh->triangles, sizeof(int) * 3 * new_face_count);
	}
	else
	{
		ERROR("new_face_count is zero.\n");
	}
	mesh->triangle_count = new_face_count;
	mesh->vertex_count   = vertex_count;
	fclose(fp);
}

void mesh_get_bounds(mesh_t *mesh, double *min, double *max)
{
	double *vertex;
	min[0] = INFINITY;
	min[1] = INFINITY;
	min[2] = INFINITY;

	max[0] = -INFINITY;
	max[1] = -INFINITY;
	max[2] = -INFINITY;
	for(int i = 0;i < mesh->vertex_count;i++)
	{
		vertex = mesh->vertices + i * 3;
		for(int j = 0; j < 3; j++)
		{
			if(vertex[j] < min[j]) min[j] = vertex[j];	
			if(vertex[j] > max[j]) max[j] = vertex[j];
		}
	}
}

int add_vertex(mesh_t *mesh, double vertices[3])
{
	double *new = mesh->vertices + mesh->vertex_count * 3;
	mesh->vertex_count++;
	mesh->vertices = realloc(mesh->vertices, sizeof(double) * mesh->vertex_count * 3);
	memcpy(new, vertices, sizeof(double) * 3);
	return mesh->triangle_count - 1;
}

void add_triangle(mesh_t *mesh, int triangle[3])
{
	int *new = mesh->triangles + mesh->triangle_count * 3;
	mesh->triangle_count++;
	mesh->triangles = realloc(mesh->triangles, sizeof(int) * mesh->vertex_count * 3);
	memcpy(new, triangle, sizeof(int) * 3);
}

int location(double *vec, double split, int axis)
{
	if(vec[axis] < split)
	{
		return -1;
	}
	else if(vec[axis] > split)
	{
		return 1;
	}
	return 0;
}

void calculate_new_point(double A[3],double B[3],double  split,int axis,double new_point[3])
{
	vector_sub(A, B, new_point);
	double t = (split - A[axis]) / new_point[axis];
	for(int i = 0; i < 3; i++)
	{
		new_point[i] *= t;
		new_point[i] += A[i];
	}
}

void mesh_split_triangle(mesh_t *mesh, int tri_index, double split, int axis)
{
	int lcount = 0, rcount = 0, ccount = 0;
	double *tri[3];
	double *left[3]   = {NULL, NULL, NULL};
	double *right[3]  = {NULL, NULL, NULL};
	int left_indices[3];
	int center_indices[3];
	int right_indices[3];

	int    indices[3];
	int    locations[3];
	mesh_get_triangle(mesh, tri_index, tri);
	mesh_get_triangle_indices(mesh, tri_index, indices);
	for(int i = 0; i < 3; i++)
	{
		locations[i] = location(tri[i], split, axis);
		if(locations[i] == -1)
		{
			left_indices[lcount] = indices[i];
			left[lcount++] = tri[i];
		}
		else if(locations[i] == 0)
		{
			center_indices[ccount] = indices[i];
			ccount++;
		}
		else
		{
			right_indices[rcount] = indices[i];
			right[rcount++] = tri[i];
		}
	}

	double *origin = NULL;
	int    origin_index;
	double **edges;
	int    *edge_indices;
	double new_point[3];
	int    new_triangle[3];
	int    *original_indices = mesh->triangles + tri_index * 3;
	if(!ccount)
	{
		if(lcount == 2)
		{
			origin = right[0];
			origin_index = right_indices[0];
			edges  = left;
			edge_indices = left_indices;
		}
		else if(rcount == 2)
		{
			origin = left[0];
			origin_index = left_indices[0];
			edges  = right;
			edge_indices = right_indices;
		}

		if(origin)
		{
			int i1, i2;
			//Caclulate the new points and add them to the mesh.
			calculate_new_point(origin, edges[0], split, axis, new_point);
			i1 = add_vertex(mesh, new_point);
			calculate_new_point(origin, edges[1], split, axis, new_point);
			i2 = add_vertex(mesh, new_point);
			//Add the new triangles indices to the list.
			original_indices[0] = origin_index;
			original_indices[1] = i1;
			original_indices[2] = i2;

			new_triangle[0] = i1;
			new_triangle[1] = edge_indices[0];
			new_triangle[2] = edge_indices[1];
			add_triangle(mesh, new_triangle);

			new_triangle[0] = edge_indices[0];
			new_triangle[1] = edge_indices[1];
			new_triangle[2] = i2;
			add_triangle(mesh, new_triangle);
		}
	}
	if(ccount == 1 && (lcount == 1 || rcount == 1))
	{
		calculate_new_point(left[0], right[0], split, axis, new_point);
		int i1 = add_vertex(mesh, new_point);
		original_indices[0] = center_indices[0];
		original_indices[1] = i1;
		original_indices[2] = left_indices[0];

		new_triangle[0] = center_indices[0];
		new_triangle[1] = i1;
		new_triangle[2] = right_indices[0];
		add_triangle(mesh, new_triangle);
	}
}

/* Functions needed to be an object. */
static void get_surface_normal(mesh_t *mesh, intersection_t *info, double out[3]);
static void get_texture_coord(mesh_t *mesh, intersection_t *info, double *tex);
static int  intersection(mesh_t *mesh, ray_t *ray, intersection_t *info);

static void set_function_pointers(mesh_t *m)
{
	m->get_normal   = (normal_func)       get_surface_normal;
	m->intersection = (intersection_func) intersection;
	m->get_tex      = (tex_func)          get_texture_coord;
	//TODO:Need to shade.
	//m->shade        = (shade_func)        shade;
	m->delete       = (delete_func)       mesh_delete;
}

static void transform(mesh_t *m)
{
	for(int i = 0; i < m->vertex_count; i++)
	{
		matrix_mult_vec(&m->transform, &m->vertices[i * 3], &m->vertices[i * 3]);
		matrix_mult_norm(&m->transform, &m->normals[i * 3], &m->normals[i * 3]);
	}
}

mesh_t *mesh_read(const char *filename)
{
	FILE *fp = OPEN(filename, "r");
	mesh_t *mesh = calloc(1, sizeof(mesh_t));
	char buf[1024];
	char mat_buffer[1024];
	mesh->normals               = NULL;
	double translate[3];
	double theta;

	matrix_identity(&mesh->transform);
	while(fgets(buf, 1024, fp))
	{
		if(sscanf(buf, "file %s", buf) == 1)
		{
			read_ply(buf, mesh);
		}
		else if(sscanf(buf, "texture %s", buf) == 1)
		{
			mesh->material.texture = texture_read(buf);
		}
		else if(sscanf(buf, "material %s", mat_buffer) == 1)
		{
			material_init(&mesh->material, mat_buffer);
		}
		else if(sscanf(buf, "translate %lf %lf %lf", translate, translate + 1, translate + 2) == 3)
		{
			matrix_translate(&mesh->transform, translate);
		}
		else if(sscanf(buf, "rotate x %lf", &theta) == 1)
		{
			matrix_rotate_x(&mesh->transform, theta);
		}
		else if(sscanf(buf, "rotate y %lf", &theta) == 1)
		{
			matrix_rotate_y(&mesh->transform, theta);
		}
		else if(sscanf(buf, "rotate z %lf", &theta) == 1)
		{
			matrix_rotate_z(&mesh->transform, theta);
		}
		else
		{
			ERROR("Invalid option reading in mesh :%s\n", filename);
		}
	}
	for(int i = 0; i < 3; i++)
	{
		if(mesh->material.reflectivity[i] + mesh->material.refractivity[i] >= 1.0)
		{
			mesh->material.refractivity[i] = 1.0 - mesh->material.reflectivity[i];
		}
		mesh->material.diffuse[i] = 1.0 - (mesh->material.reflectivity[i] + mesh->material.refractivity[i]);
	}
	set_function_pointers(mesh);
	fclose(fp);
	transform(mesh);
	mesh->tree           = kdtree_build(mesh);
	return mesh;
}


static int  intersection(mesh_t *mesh, ray_t *ray, intersection_t *info)
{
	return intersection_ray_mesh(ray, mesh, info);
}


static void get_surface_normal(mesh_t *mesh, intersection_t *info, double out[3])
{
	int tri = info->mesh.triangle;
	double u = info->triangle.u;
	double v = info->triangle.v;
	double b[3] = {1 - u - v, u, v};
	memset(out, 0x00, sizeof(*out) * 3);
	for(int i = 0; i < 3; i++)
	{
		out[0] += (MESH_NORMAL(mesh, tri, i)[0] * b[i]);
		out[1] += (MESH_NORMAL(mesh, tri, i)[1] * b[i]);
		out[2] += (MESH_NORMAL(mesh, tri, i)[2] * b[i]);
	}
}

void mesh_calculate_ambient_colour(mesh_t  *mesh, scene_t *scene, ray_t *ray, intersection_t *info, double *colour_out)
{}

void mesh_calculate_texcoords(mesh_t *mesh,int triangle,double u,double v,double coords[2])
{
	memset(coords, 0x00, sizeof(*coords) * 2);
	double b[3] = {1 - u - v, u, v};
	for(int i = 0; i < 3; i++)
	{
		coords[0] += MESH_TEX(mesh, triangle, i)[0] * b[i];
		coords[1] += MESH_TEX(mesh, triangle, i)[1] * b[i];
	}
}

static void get_texture_coord(mesh_t *mesh, intersection_t *info, double *tex)
{
	mesh_calculate_texcoords(mesh, info->mesh.triangle, info->triangle.u, info->triangle.v, tex);
}

void mesh_calculate_texture_colour(mesh_t *mesh, int triangle, double u, double v, double colour[3])
{
	double coords[2];
	mesh_calculate_texcoords(mesh, triangle, u, v, coords);
	if(mesh->material.texture)
	{
		texture_sample(mesh->material.texture, coords[0], coords[1], colour);
	}
	else
	{
		memcpy(colour, mesh->material.diffuse, sizeof(*colour) * 3);
	}
}


void mesh_calculate_refractive_colour(mesh_t  *mesh, scene_t *scene, ray_t *ray, intersection_t *info, double *colour_out)
{
}

void mesh_delete(mesh_t *mesh)
{
	free(mesh->triangles);
	free(mesh->vertices);
	if(mesh->normals)
	{
		free(mesh->normals);
	}
	if(mesh->tex)
	{
		free(mesh->tex);
	}
	if(mesh->material.texture)
	{
		texture_delete(mesh->material.texture);
	}
	kdtree_delete(mesh->tree);
	free(mesh);
}
