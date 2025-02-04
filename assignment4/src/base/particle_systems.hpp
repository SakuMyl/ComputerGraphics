#pragma once

#include "../framework/base/Math.hpp"

#include <vector>

// EXTRA: probably want to use Eigen for the implicit solver

//#include "../Eigen/SparseCore" // for the sparse matrices
//#include "../Eigen/SparseLU"   // LU decomposition for linear solves
//#include "../Eigen/StdVector"  // for interop with the State vectors

// less writing
// typedef Eigen::SparseMatrix < float, 0, Eigen::DenseIndex > SparseMatrix;
// typedef Eigen::SparseLU<SparseMatrix, Eigen::COLAMDOrdering<Eigen::DenseIndex>> SparseLU;

// Eigen supports a multitude of solvers, SparseLU is probably the best fit for us since our
// problems are sparse but not necessarily symmetric or positive definite.

typedef std::vector<FW::Vec3f> State;
typedef std::vector<FW::Vec3f> Points;
typedef std::vector<FW::Vec3f> Lines;

struct Spring {
	Spring() {}
	Spring(unsigned index1, unsigned index2, float spring_k, float rest_length) :
		i1(index1), i2(index2), k(spring_k), rlen(rest_length) {}
	unsigned i1, i2;
	float k, rlen;
};

class ParticleSystem {
public:
	virtual					~ParticleSystem() {};
	virtual State			evalF(const State&) const = 0;
#ifdef EIGEN_SPARSECORE_MODULE_H
	virtual	void			evalJ(const State&, SparseMatrix& result, bool initial) const = 0;
#endif
	virtual void			reset() = 0;
	const State&			state() { return state_; }
	void					set_state(State s) { state_ = s; }
	virtual void			toggle_wind() {};
	virtual void			step() {};
	virtual void			toggle_plane() {};
	virtual Points			getPoints() { return Points(); }
	virtual Lines			getLines() { return Lines(); }
protected:
	State					state_;
private:
	bool					wind_;
};

class SimpleSystem : public ParticleSystem {
public:
	SimpleSystem() : radius_(0.5f) { reset(); }
	State					evalF(const State&) const override;
#ifdef EIGEN_SPARSECORE_MODULE_H
	void					evalJ(const State&, SparseMatrix& result, bool initial) const override;
#endif
	void					reset() override;
	Points					getPoints() override;
	Lines					getLines() override;

private:
	float					radius_;
};

class SpringSystem : public ParticleSystem {
public:
	SpringSystem() { reset(); }
	State					evalF(const State&) const override;
#ifdef EIGEN_SPARSECORE_MODULE_H
	void					evalJ(const State&, SparseMatrix& result, bool initial) const override;
#endif
	void					reset() override;
	Points					getPoints() override;
	Lines					getLines() override;

private:
	Spring					spring_;
};

class PendulumSystem : public ParticleSystem {
public:
	PendulumSystem(unsigned n) : n_(n) { reset(); }
	State					evalF(const State&) const override;
#ifdef EIGEN_SPARSECORE_MODULE_H
	void					evalJ(const State&, SparseMatrix& result, bool initial) const override;
#endif
	void					reset() override;
	Points					getPoints() override;
	Lines					getLines() override;

private:
	unsigned				n_;
	std::vector<Spring>		springs_;
};

class ClothSystem : public ParticleSystem {
public:
	ClothSystem(unsigned x, unsigned y) : x_(x), y_(y) { reset(); }
	State					evalF(const State&) const override;
#ifdef EIGEN_SPARSECORE_MODULE_H
	void					evalJ(const State&, SparseMatrix& result, bool initial) const override;
#endif
	void					reset() override;
	virtual void			toggle_wind() { wind_ = !wind_; reset_wind(); }
	virtual void			reset_wind();
	virtual void			step() override;
	Points					getPoints() override;
	Lines					getLines() override;
	FW::Vec2i				getSize() { return FW::Vec2i(x_, y_); }

private:
	unsigned				x_, y_;
	bool					wind_;
	FW::Vec3f				wind_dir_;
	FW::Vec3f				target_dir_;
	FW::Vec3f				initial_dir_;
	int						step_;
	std::vector<Spring>		springs_;
};

class FluidSystem : public ParticleSystem {
public:
							FluidSystem(unsigned n) : n_(n) { reset(); }
							FluidSystem() { reset(); }
	State					evalF(const State&) const override;
#ifdef EIGEN_SPARSECORE_MODULE_H
	void					evalJ(const State&, SparseMatrix& result, bool initial) const override;
#endif
	void					reset() override;
	void					step() override;
	virtual void			toggle_plane() { plane_ = !plane_; }
	Points					getPoints() override;
	Lines					getLines() { return Lines(); };

private:
	unsigned				n_;
	bool					plane_;
	std::vector<unsigned>	ages_;
};
