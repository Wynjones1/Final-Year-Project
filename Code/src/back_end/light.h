#ifndef LIGHT_H
#define LIGHT_H

enum light_type
{
	light_type_invalid,
	light_type_point,
	light_type_area,
};

struct light
{
	enum light_type type;
	double origin[3];
	double power[3];
	double av_power;
	union
	{
		struct
		{
			double width;
			double height;
			double normal[3];
		};
	};
};

typedef struct light light_t;
typedef struct scene scene_t;
typedef struct ray   ray_t;

/* Creates a new light from an input string. */
light_t *light_new(const char *buf);

/* returns the percentage in light from 0.0 1.0 */
double light_is_blocked(light_t *l, scene_t *scene, double point[3]);
void light_delete(light_t *light);

/* Generates a ray that eminated from the light */
void light_generate_ray(light_t *l, ray_t *ray);
void light_calculate_radiance(light_t *l, scene_t *scene, double point[3], double normal[3],double radiance[3]);

#endif
