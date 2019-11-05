
#include "utility.hpp"
#include "particle_systems.hpp"
#include "integrators.hpp"

void eulerStep(ParticleSystem& ps, float step) {
	ps.add_particles();
	State f = ps.evalF(ps.state());
	State old_state = ps.state();
	State new_state(old_state.size());
	for (int i = 0; i < old_state.size(); ++i) {
		new_state[i] = old_state[i] + step * f[i];
	}
	ps.set_state(new_state);
};

void trapezoidStep(ParticleSystem& ps, float step) {
	ps.add_particles();
	State s0 = ps.state();
	State f0 = ps.evalF(s0);
	State s1(f0.size());
	for (int i = 0; i < s0.size(); ++i) {
		s1[i] = s0[i] + step * f0[i];
	}
	State f1 = ps.evalF(s1);
	State new_state(s0.size());
	for (int i = 0; i < s0.size(); ++i) {
		new_state[i] = s0[i] + 0.5f * step * (f0[i] + f1[i]);
	}
	ps.set_state(new_state);
	// YOUR CODE HERE (R3)
	// Implement a trapezoid integrator.
}

void midpointStep(ParticleSystem& ps, float step) {
	ps.add_particles();
	const auto& x0 = ps.state();
	auto n = x0.size();
	auto f0 = ps.evalF(x0);
	auto xm = State(n), x1 = State(n);
	for (auto i = 0u; i < n; ++i) {
		xm[i] = x0[i] + (0.5f * step) * f0[i];
	}
	auto fm = ps.evalF(xm);
	for (auto i = 0u; i < n; ++i) {
		x1[i] = x0[i] + step * fm[i];
	}
	ps.set_state(x1);
}

void rk4Step(ParticleSystem& ps, float step) {
	ps.add_particles();
	State initial_state = ps.state();
	State k1 = ps.evalF(initial_state);
	for (auto& f : k1) f *= step;
	State state2(initial_state.size());
	State state3(initial_state.size());
	State state4(initial_state.size());
	State final_state(initial_state.size());
	for (int i = 0; i < initial_state.size(); ++i) {
		state2[i] = initial_state[i] + 0.5f * k1[i];
	}
	State k2 = ps.evalF(state2);
	for (auto& f : k2) f *= step;
	for (int i = 0; i < initial_state.size(); ++i) {
		state3[i] = initial_state[i] + 0.5f * k2[i];
	}
	State k3 = ps.evalF(state3);
	for (auto& f : k3) f *= step;
	for (int i = 0; i < initial_state.size(); ++i) {
		state4[i] = initial_state[i] + k3[i];
	}
	State k4 = ps.evalF(state4);
	for (auto& f : k4) f *= step;
	for (int i = 0; i < initial_state.size(); ++i) {
		final_state[i] = initial_state[i] + (1.0f / 6) * (k1[i] + 2.0f * k2[i] + 2.0f * k3[i] + k4[i]);
	}
	ps.set_state(final_state);
}

#ifdef EIGEN_SPARSECORE_MODULE_H

void implicit_euler_step(ParticleSystem& ps, float step, SparseMatrix& J, SparseLU& solver, bool initial) {
	// EXTRA: Implement the implicit Euler integrator. (Note that the related formula on page 134 on the lecture slides is missing a 'h'; the formula should be (I-h*Jf(Yi))DY=-F(Yi))
}

void implicit_midpoint_step(ParticleSystem& ps, float step, SparseMatrix& J, SparseLU& solver, bool initial) {
	// EXTRA: Implement the implicit midpoint integrator.
}
#endif
