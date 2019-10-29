
#include "utility.hpp"
#include "particle_systems.hpp"
#include "integrators.hpp"

void eulerStep(ParticleSystem& ps, float step) {
	State f = ps.evalF(ps.state());
	State old_state = ps.state();
	State new_state(old_state.size());
	for (int i = 0; i < old_state.size(); ++i) {
		new_state[i] = old_state[i] + step * f[i];
	}
	ps.set_state(new_state);
};

void trapezoidStep(ParticleSystem& ps, float step) {
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
	// EXTRA: Implement the RK4 Runge-Kutta integrator.
}

#ifdef EIGEN_SPARSECORE_MODULE_H

void implicit_euler_step(ParticleSystem& ps, float step, SparseMatrix& J, SparseLU& solver, bool initial) {
	// EXTRA: Implement the implicit Euler integrator. (Note that the related formula on page 134 on the lecture slides is missing a 'h'; the formula should be (I-h*Jf(Yi))DY=-F(Yi))
}

void implicit_midpoint_step(ParticleSystem& ps, float step, SparseMatrix& J, SparseLU& solver, bool initial) {
	// EXTRA: Implement the implicit midpoint integrator.
}
#endif
