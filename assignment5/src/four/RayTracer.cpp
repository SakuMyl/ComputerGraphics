#include "raytracer.hpp"

#include "args.hpp"
#include "Camera.h"
#include "hit.hpp"
#include "lights.hpp"
#include "material.hpp"
#include "objects.hpp"
#include "ray.hpp"
#include "SceneParser.h"

#define EPSILON 0.001f

using namespace std;
using namespace FW;

namespace {

// Compute the mirror direction for the incoming direction, given the surface normal.
Vec3f mirrorDirection(const Vec3f& normal, const Vec3f& incoming) {
	// YOUR CODE HERE (R8)
	// Pay attention to the direction which things point towards, and that you only
	// pass in normalized vectors.
	return (incoming - 2 * incoming.dot(normal) * normal).normalized();
}

bool transmittedDirection(const Vec3f& normal, const Vec3f& incoming, 
		float index_i, float index_t, Vec3f& transmitted) {
	// YOUR CODE HERE (EXTRA)
	// Compute the transmitted direction for the incoming direction, given the surface normal
	// and indices of refraction. Pay attention to the direction which things point towards!
	// You should only pass in normalized vectors!
	// The function should return true if the computation was successful, and false
	// if the transmitted direction can't be computed due to total internal reflection.
	float nr = index_i / index_t;
	float ndoti = normal.dot(-incoming);
	float d = 1 - nr * nr * (1 - ndoti * ndoti);
	if (d < 0) return false;
	transmitted = (nr * ndoti - sqrtf(d)) * normal + nr * incoming;
	return true;
}

} // namespace

Vec3f RayTracer::traceRay(Ray& ray, float tmin, int bounces, float refr_index, Hit& hit, FW::Vec3f debug_color) const {
	// initialize a hit to infinitely far away
	hit = Hit(FLT_MAX);

	// Ask the root node (the single "Group" in the scene) for an intersection.
	bool intersect = false;
	if(scene_.getGroup())
		intersect = scene_.getGroup()->intersect(ray, hit, tmin);

	// Write out the ray segment if visualizing (for debugging purposes)
	if (debug_trace)
		debug_rays.push_back(RaySegment(ray.origin, ray.direction.normalized()*min(100.0f,hit.t), hit.normal, debug_color));

	// if the ray missed, we return the background color.
	if (!intersect)
		return scene_.getBackgroundColor();
	
	Material* m = hit.material;
	assert(m != nullptr);

	// get the intersection point and normal.
	Vec3f normal = hit.normal;
	Vec3f point = ray.pointAtParameter(hit.t);

	// YOUR CODE HERE (R1)
	// Apply ambient lighting using the ambient light of the scene
	// and the diffuse color of the material.
	Vec3f answer = m->diffuse_color(point) * scene_.getAmbientLight();

	// YOUR CODE HERE (R4 & R7)
	// For R4, loop over all the lights in the scene and add their contributions to the answer.
	// If you wish to include the shadow rays in the debug visualization, insert the segments
	// into the debug_rays list similar to what is done to the primary rays after intersection.
	// For R7, if args_.shadows is on, also shoot a shadow ray from the hit point to the light
	// to confirm it isn't blocked; if it is, ignore the contribution of the light.
	auto lights = scene_.getNumLights();
	for (int i = 0; i < lights; ++i) {
		auto light = scene_.getLight(i);
		Vec3f dir_to_light, incident_intensity;
		float distance;
		light->getIncidentIllumination(point, dir_to_light, incident_intensity, distance);
		bool add = true;
		if (args_.shadows) {
			Ray r = Ray(point, dir_to_light);
			Hit h = Hit(distance);
			auto objs = scene_.getGroup();
			if (objs->intersect(r, h, 0.0001f)) add = false;
		}
		if(add) answer += m->shade(ray, hit, dir_to_light, incident_intensity, args_.shade_back);
	}
	// are there bounces left?
	if (bounces >= 1) {
		// reflection, but only if reflective coefficient > 0!
		if (m->reflective_color(point).length() > 0.0f) {
			// YOUR CODE HERE (R8)
			// Generate and trace a reflected ray to the ideal mirror direction and add
			// the contribution to the result. Remember to modulate the returned light
			// by the reflective color of the material of the hit point.
			Vec3f dir = mirrorDirection(normal, ray.direction);
			Ray r(point, dir);
			Hit h;
			answer += m->reflective_color(point) * traceRay(r, 0.0001f, bounces - 1, refr_index, h, debug_color);
		}

		// refraction, but only if surface is transparent!
		if (m->transparent_color(point).length() > 0.0f) {
			// YOUR CODE HERE (EXTRA)
			Vec3f n = normal.dot(ray.direction) < 0 ? normal : -normal;
			//If n and normal point to the same direction i.e. are the same,
			//the ray is entering a material, otherwise exiting
			float n2 = n.dot(normal) > 0 ? m->refraction_index(point) : 1;
			Vec3f dir;
			if (transmittedDirection(n, ray.direction, refr_index, n2, dir)) {
				Ray r(point, dir.normalized());
				Hit h;
				answer += m->transparent_color(point) * traceRay(r, 0.0001f, bounces - 1, n2, h, debug_color);
			}
			// Generate a refracted direction and trace the ray. For this, you need
			// the index of refraction of the object. You should consider a ray going through
			// the object "against the normal" to be entering the material, and a ray going
			// through the other direction as exiting the material to vacuum (refractive index=1).
			// (Assume rays always start in vacuum, and don't worry about multiple intersecting
			// refractive objects!) Remembering this will help you figure out which way you
			// should use the material's refractive index. Remember to modulate the result
			// with the material's refractiveColor().
			// REMEMBER you need to account for the possibility of total internal reflection as well.
		}
	}
	return answer;
}
