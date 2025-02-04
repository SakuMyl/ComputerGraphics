#include "objects.hpp"

#include "hit.hpp"
#include "VecUtils.h"

#include <cassert>

using namespace std;
using namespace FW;

Object3D* Group::operator[](int i) const {
	assert(i >= 0 && size_t(i) < size());
	return objects_[i].get();
}

void Group::insert(Object3D* o) {
	assert(o);
	objects_.emplace_back(o);
}

bool Group::intersect(const Ray& r, Hit& h, float tmin) const {
	// We intersect the ray with each object contained in the group.
	bool intersected = false;
	for (int i = 0; i < int(size()); ++i) {
		Object3D* o = objects_[i].get();
		assert(o != nullptr);
		assert(h.t >= tmin);
		bool tmp = o->intersect(r, h, tmin);
		assert(h.t >= tmin);
		if (tmp)
			intersected = true;
	}
	assert(h.t >= tmin);
	return intersected;
}

bool Box::intersect(const Ray& r, Hit& h, float tmin) const {
// YOUR CODE HERE (EXTRA)
// Intersect the box with the ray!
	Vec3f dir = r.direction;
	Vec3f starts = (min_ - r.origin) / dir;
	Vec3f ends = (max_ - r.origin) / dir;
	float start = starts.max();
	float end = ends.min();
	Vec3f n;
	if (start < end && end >= tmin) {
		float a = 1;
		float t = end;
		//In case the ray's origin is inside the box, the normal will point to the same
		//direction as the respective component of ray.direction, otherwise opposite
		if (start >= tmin) {
			a = -1;
			t = start;
		}
		if (start == starts.x) {
			n = Vec3f(dir.x > 0 ? a : -a, 0, 0);
		}
		else if (start == starts.y) {
			n = Vec3f(0, dir.y > 0 ? a : -a, 0);
		}
		else {
			n = Vec3f(0, 0, dir.z > 0 ? a : -a);
		}
		h.set(t, material(), n);
		return true;
	}
	return false;
}

bool Plane::intersect( const Ray& r, Hit& h, float tmin ) const {
	// YOUR CODE HERE (R5)
	// Intersect the ray with the plane.
	// Pay attention to respecting tmin and h.t!
	// Equation for a plane:
	// ax + by + cz = d;
	// normal . p - d = 0
	// (plug in ray)
	// origin + direction * t = p(t)
	// origin . normal + t * direction . normal = d;
	// t = (d - origin . normal) / (direction . normal);
	auto t = (offset_ - normal_.dot(r.origin)) / (normal_.dot(r.direction));
	if (t < h.t && t >= tmin) {
		h.set(t, material(), normal_);
		return true;
	}
	return false;
}

Transform::Transform(const Mat4f& m, Object3D* o) :
	matrix_(m),
	object_(o)
{
	assert(o != nullptr);
	inverse_ = matrix_.inverted();
	inverse_transpose_ = inverse_.transposed();
}

bool Transform::intersect(const Ray& r, Hit& h, float tmin) const {
	// YOUR CODE HERE (EXTRA)
	// Transform the ray to the coordinate system of the object inside,
	// intersect, then transform the normal back. If you don't renormalize
	// the ray direction, you can just keep the t value and do not need to
	// recompute it!
	// Remember how points, directions, and normals are transformed differently!
	Vec3f origin = (inverse_ * Vec4f(r.origin, 1)).getXYZ();
	Vec3f direction = (inverse_ * Vec4f(r.direction, 0)).getXYZ();
	Ray ray = Ray(origin, direction);
	bool ret = object_->intersect(ray, h, tmin);
	if (ret) h.set(h.t, object_->material(), (inverse_transpose_ * h.normal).normalized());
	return ret; 
}

bool Sphere::intersect( const Ray& r, Hit& h, float tmin ) const {
	// Note that the sphere is not necessarily centered at the origin.
	
	Vec3f tmp = center_ - r.origin;
	Vec3f dir = r.direction;

	float A = dot(dir, dir);
	float B = - 2 * dot(dir, tmp);
	float C = dot(tmp, tmp) - sqr(radius_);
	float radical = B*B - 4*A*C;
	if (radical < 0)
		return false;

	radical = sqrtf(radical);
	float t_m = ( -B - radical ) / ( 2 * A );
	float t_p = ( -B + radical ) / ( 2 * A );
	Vec3f pt_m = r.pointAtParameter( t_m );
	Vec3f pt_p = r.pointAtParameter( t_p );

	assert(r.direction.length() > 0.9f);

	bool flag = t_m <= t_p;
	if (!flag) {
		::printf( "sphere ts: %f %f %f\n", tmin, t_m, t_p );
		return false;
	}
	assert( t_m <= t_p );

	// choose the closest hit in front of tmin
	float t = (t_m < tmin) ? t_p : t_m;

	if (h.t > t  && t > tmin) {
		Vec3f normal = r.pointAtParameter(t);
		normal -= center_;
		normal.normalize();
		h.set(t, this->material(), normal);
		return true;
	}
	return false;
} 

Triangle::Triangle(const Vec3f& a, const Vec3f& b, const Vec3f& c,
	Material *m, const Vec2f& ta, const Vec2f& tb, const Vec2f& tc, bool load_mesh) :
	Object3D(m)
{
	vertices_[0] = a;
	vertices_[1] = b;
	vertices_[2] = c;
	texcoords_[0] = ta;
	texcoords_[1] = tb;
	texcoords_[2] = tc;

	if (load_mesh) {
		preview_mesh.reset((FW::Mesh<FW::VertexPNT>*)FW::importMesh("preview_assets/tri.obj"));
		set_preview_materials();
	}
}

bool Triangle::intersect( const Ray& r, Hit& h, float tmin ) const {
	// YOUR CODE HERE (R6)
	// Intersect the triangle with the ray!
	// Again, pay attention to respecting tmin and h.t!
	auto v_a = vertices_[0];
	auto v_b = vertices_[1];
	auto v_c = vertices_[2];
	Mat3f m;
	auto dir = r.direction;
	m.setCol(0, v_a - v_b);
	m.setCol(1, v_a - v_c);
	m.setCol(2, dir);
	Vec3f answer = m.inverted() * (v_a - r.origin);
	auto b = answer.x, c = answer.y, t = answer.z;
	if (b > 0 && c > 0 && b + c < 1 && h.t > t && t >= tmin) {
		Vec3f normal = (v_b - v_a).cross(v_c - v_a).normalized();
		h.set(t, material(), normal);
		return true;
	}
	return false;
}

const Vec3f& Triangle::vertex(int i) const {
	assert(i >= 0 && i < 3);
	return vertices_[i];
}


