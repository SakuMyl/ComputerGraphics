#include "particle_systems.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>

using namespace std;
using namespace FW;

namespace {

	inline Vec3f fGravity(float mass) {
		return Vec3f(0, -9.8f * mass, 0);
	}

	// force acting on particle at pos1 due to spring attached to pos2 at the other end
	inline Vec3f fSpring(const Vec3f& pos1, const Vec3f& pos2, float k, float rest_length) {
		// YOUR CODE HERE (R2)
		Vec3f d = pos2 - pos1;
		float len = d.length();
		return -k * (len - rest_length) * d / len;
	}

	inline Vec3f fDrag(const Vec3f& v, float k) {
		// YOUR CODE HERE (R2)
		return -k * v;
	}

	inline Vec3f fWind(Vec3f direction) {
		return 0.5f * direction;
	}

} // namespace

void SimpleSystem::reset() {
	state_ = State(1, Vec3f(0, radius_, 0));
}

State SimpleSystem::evalF(const State& state) const {
	State f(1, Vec3f(-state[0].y, state[0].x, 0));
	return f;
}

#ifdef EIGEN_SPARSECORE_MODULE_H
// using the implicit Euler method, the simple system should converge towards origin -- as opposed to the explicit Euler, which diverges outwards from the origin.
void SimpleSystem::evalJ(const State&, SparseMatrix& result, bool initial) const {
	if (initial) {
		result.coeffRef(1, 0) = 1.0f;
		result.coeffRef(0, 1) = -1.0f;
	}
}
#endif

Points SimpleSystem::getPoints() {
	return Points(1, state_[0]);
}

Lines SimpleSystem::getLines() {
	static const auto n_lines = 50u;
	auto l = Lines(n_lines * 2);
	const auto angle_incr = 2 * FW_PI / n_lines;
	for (auto i = 0u; i < n_lines; ++i) {
		l[2 * i] = l[2 * i + 1] =
			Vec3f(radius_ * FW::sin(angle_incr * i), radius_ * FW::cos(angle_incr * i), 0);
	}
	rotate(l.begin(), l.begin() + 1, l.end());
	return l;
}

void SpringSystem::reset() {
	const auto start_pos = Vec3f(0.1f, -0.5f, 0.0f);
	const auto spring_k = 30.0f;
	state_ = State(4);
	state_[0] = state_[1] = state_[3] = Vec3f(0);
	state_[2] = start_pos;
	spring_.k = spring_k;
	spring_.rlen = start_pos.length();
	// YOUR CODE HERE (R2)
	// Set the initial state for a particle system with one particle fixed
	// at origin and another particle hanging off the first one with a spring.
	// Place the second particle initially at start_pos.
}

State SpringSystem::evalF(const State& state) const {
	const auto drag_k = 0.5f;
	const auto mass = 1.0f;
	State f(4);
	f[0] = f[1] = Vec3f(0);
	f[2] = state[3];
	f[3] = (fGravity(mass) + fSpring(state[0], state[2], spring_.k, spring_.rlen) + fDrag(state[3], drag_k)) / mass;
	// YOUR CODE HERE (R2)
	// Return a derivative for the system as if it was in state "state".
	// You can use the fGravity, fDrag and fSpring helper functions for the forces.

	return f;
}

#ifdef EIGEN_SPARSECORE_MODULE_H

// This is a very useful read for the Jacobians of the spring forces. It deals with spring damping as well, we don't do that -- our drag is simply a linear damping of velocity (that results in some constants in the Jacobian).
// http://blog.mmacklin.com/2012/05/04/implicitsprings/

void SpringSystem::evalJ(const State& state, SparseMatrix& result, bool initial) const {
	const auto drag_k = 0.5f;
	const auto mass = 1.0f;
	// EXTRA: Evaluate the Jacobian into the 'result' matrix here. Only the free end of the spring should have any nonzero values related to it.
}
#endif

Points SpringSystem::getPoints() {
	auto p = Points(2);
	p[0] = state_[0]; p[1] = state_[2];
	return p;
}

Lines SpringSystem::getLines() {
	auto l = Lines(2);
	l[0] = state_[0]; l[1] = state_[2];
	return l;
}

void PendulumSystem::reset() {
	const auto spring_k = 1000.0f;
	const auto start_point = Vec3f(0);
	const auto end_point = Vec3f(0.05, -1.5, 0);
	state_ = State(2 * n_);
	auto step = end_point / n_;
	auto steplen = step.length();
	vector<Spring> springs(n_ - 1);
	for (int i = 0; i < n_; ++i) {
		state_[2 * i] = (float)i * step;
		state_[2 * i + 1] = 0;
		if (i != n_ - 1) {
			Spring s;
			s.rlen = steplen;
			s.k = spring_k;
			s.i1 = i;
			s.i2 = i + 1;
			springs[i] = s;
		}
	}
	springs_ = springs;
	// YOUR CODE HERE (R4)	
	// Set the initial state for a pendulum system with n_ particles
	// connected with springs into a chain from start_point to end_point.
}

State PendulumSystem::evalF(const State& state) const {
	const auto drag_k = 0.5f;
	const auto mass = 0.5f;
	auto f = State(2 * n_);
	auto gravity = fGravity(mass);
	for (int i = 1; i < n_; ++i) {
		f[2 * i] = state[2 * i + 1];
		f[2 * i + 1] = gravity + fDrag(state[2 * i + 1], drag_k);
	}
	for (auto s : springs_) {
		f[2 * s.i1 + 1] += fSpring(state[2 * s.i2], state[2 * s.i1], s.k, s.rlen);
		f[2 * s.i2 + 1] += fSpring(state[2 * s.i1], state[2 * s.i2], s.k, s.rlen);
	}
	f[0] = f[1] = 0;
	//Divide by mass to get acceleration
	for (auto &f_i : f) {
		f_i = f_i / mass;
	}
	// YOUR CODE HERE (R4)
	// As in R2, return a derivative of the system state "state".
	return f;
}

#ifdef EIGEN_SPARSECORE_MODULE_H

void PendulumSystem::evalJ(const State& state, SparseMatrix& result, bool initial) const {

	const auto drag_k = 0.5f;
	const auto mass = 0.5f;

	// EXTRA: Evaluate the Jacobian here. Each spring has an effect on four blocks of the matrix -- both of the positions of the endpoints will have an effect on both of the velocities of the endpoints.
}
#endif


Points PendulumSystem::getPoints() {
	auto p = Points(n_);
	for (auto i = 0u; i < n_; ++i) {
		p[i] = state_[i * 2];
	}
	return p;
}

Lines PendulumSystem::getLines() {
	auto l = Lines();
	for (const auto& s : springs_) {
		l.push_back(state_[2 * s.i1]);
		l.push_back(state_[2 * s.i2]);
	}
	return l;
}

void ClothSystem::reset() {
	const auto spring_k = 300.0f;
	const auto width = 1.5f, height = 1.5f; // width and height of the whole grid
	state_ = State(2 * x_*y_);
	wind_ = false;
	reset_wind();
	auto horizontal_step = width / (x_ - 1);
	auto vertical_step = height / (y_ - 1);
	auto diagonal_step = sqrtf(horizontal_step * horizontal_step + vertical_step * vertical_step);
	vector<Spring> springs;
	for (int i = 0; i < y_; ++i) {
		for (int j = 0; j < x_; ++j) {
			state_[2 * (i * x_ + j)] = Vec3f(j * horizontal_step - 0.75f, 0, -i * vertical_step);
			state_[2 * (i * x_ + j) + 1] = 0;
			Spring s;
			s.k = spring_k;
			s.i1 = x_ * i + j;
			if (j != x_ - 1) {
				s.rlen = horizontal_step;
				s.i2 = s.i1 + 1;
				springs.push_back(s);
				if (j != x_ - 2) {
					s.rlen = 2 * horizontal_step;
					s.i2 = s.i1 + 2;
					springs.push_back(s);
				}
				if (i != y_ - 1) {
					s.rlen = diagonal_step;
					s.i2 = x_ * (i + 1) + j + 1;
					springs.push_back(s);
				}
				if (i != 0) {
					s.rlen = diagonal_step;
					s.i2 = x_ * (i - 1) + j + 1;
					springs.push_back(s);
				}
			}
			if (i != y_ - 1) {
				s.rlen = vertical_step;
				s.i2 = x_ * (i + 1) + j;
				springs.push_back(s);
				if (i != y_ - 2) {
					s.rlen = 2 * vertical_step;
					s.i2 = x_ * (i + 2) + j;
					springs.push_back(s);
				}
			}
		}
	}
	springs_ = springs;
	// YOUR CODE HERE (R5)
	// Construct a particle system with a x_ * y_ grid of particles,
	// connected with a variety of springs as described in the handout:
	// structural springs, shear springs and flex springs.
}

State ClothSystem::evalF(const State& state) const {
	const auto drag_k = 0.08f;
	const auto n = x_ * y_;
	static const auto mass = 0.025f;
	auto f = State(2 * n);
	auto g = fGravity(mass);
	Vec3f wind;
	for (int i = 0; i < n; ++i) {
		f[2 * i] = state[2 * i + 1];
		f[2 * i + 1] = g + fDrag(state[2 * i + 1], drag_k) + (wind_ ? fWind(wind_dir_) : 0);
	}
	for (auto s : springs_) {
		f[2 * s.i1 + 1] += fSpring(state[2 * s.i2], state[2 * s.i1], s.k, s.rlen);
		f[2 * s.i2 + 1] += fSpring(state[2 * s.i1], state[2 * s.i2], s.k, s.rlen);
	}
	f[0] = f[1] = f[2 * (x_ - 1)] = f[2 * (x_ - 1) + 1] = 0;
	//Divide by mass to get acceleration
	for (int i = 0; i < n; ++i) {
		f[2 * i + 1] = f[2 * i + 1] / mass;
	}
	// YOUR CODE HERE (R5)
	// This will be much like in R2 and R4.
	return f;
}

#ifdef EIGEN_SPARSECORE_MODULE_H

void ClothSystem::evalJ(const State& state, SparseMatrix& result, bool initial) const {
	const auto drag_k = 0.08f;
	static const auto mass = 0.025f;

	// EXTRA: Evaluate the Jacobian here. The code is more or less the same as for the pendulum.
}

#endif

void ClothSystem::step() {
	if (step_ == 0) {
		initial_dir_ = target_dir_;
		target_dir_ = Vec3f(1.0f * rand() / RAND_MAX - 0.5f, 1.0f * rand() / RAND_MAX - 0.5f, 1.0f * rand() / RAND_MAX - 0.5f);
	}
	float t = step_ / 10000.0f;
	t = (FW::cos(FW_PI * t) + 1) / 2;
	wind_dir_ = (t * initial_dir_ + (1 - t) * target_dir_);
	if (step_ == 10000) step_ = 0; else step_++;
}
void ClothSystem::reset_wind() {
	initial_dir_ = target_dir_ = wind_dir_ = 0;
	step_ = 0;
}

Points ClothSystem::getPoints() {
	auto n = x_ * y_;
	auto p = Points(n);
	for (auto i = 0u; i < n; ++i) {
		p[i] = state_[2 * i];
	}
	return p;
}

Lines ClothSystem::getLines() {
	auto l = Lines();
	for (const auto& s : springs_) {
		l.push_back(state_[2 * s.i1]);
		l.push_back(state_[2 * s.i2]);
	}
	return l;
}
State FluidSystem::evalF(const State&) const {
	auto buoyancy = -fGravity(100.0f);
	auto kDrag = 0.05f;
	State f(2 * n_);
	for (int i = 0; i < n_; ++i) {
		if (ages_[i] != 0) {
			auto pos = state_[2 * i];
			auto vel = state_[2 * i + 1];
			//The particle hits a plane if it's toggled
			if (plane_ && -pos.x + pos.y > 0.5f && pos.x < 0.5f) {
				vel = Vec3f(vel.y, vel.x, vel.z);
			}
			f[2 * i] = vel;
			f[2 * i + 1] = buoyancy + fDrag(vel, kDrag);
		}
	}
	return f;
}

void FluidSystem::step() {
	vector<unsigned> new_ages;
	State new_state;
	for (int i = 0; i < n_; ++i) {
		if (ages_[i] < 500) {
			new_ages.push_back(ages_[i] + 1);
			new_state.push_back(state_[2 * i]);
			new_state.push_back(state_[2 * i + 1]);
		}
	}
	for (int i = 0; i < 5; ++i) {
		new_ages.push_back(0);
		new_state.push_back(Vec3f(0.5f * rand() / RAND_MAX - 0.25f, 0.5f * rand() / RAND_MAX - 1.0f, 0.5f * rand() / RAND_MAX - 0.25f));
		new_state.push_back(Vec3f(10.0f * rand() / RAND_MAX - 5.0f, 10.0f * rand() / RAND_MAX, 10.0f * rand() / RAND_MAX - 5.0f));
	}
	state_ = new_state;
	ages_ = new_ages;
	n_ = ages_.size();
}

void FluidSystem::reset() {
	n_ = 0;
	plane_ = true;
}

Points FluidSystem::getPoints() {
	auto p = Points(n_);
	for (int i = 0; i < n_; ++i) {
		p[i] = state_[2 * i];
	}
	return p;
}


