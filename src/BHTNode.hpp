#pragma once

#include <map>

#include "ordinal.h"
#include "quadrant.hpp"
#include "particle.hpp"

using namespace std;

/* BH tree's node */
class BHTNode
{
private:
	// each node can contain a particle and is in a quadrant
	Particle* particle{};
	Quadrant quadrant;

	// an internal node can have up to 4 children, one for each quadrant
	map<Ordinal, BHTNode*> children;

public:
	/* constructor : initially the node is in a quadrant but does not contain a particle yet */
	BHTNode(Quadrant quadrant) : quadrant(quadrant)
	{
		children[NORTH_EAST] = nullptr;
		children[NORTH_WEST] = nullptr;
		children[SOUTH_EAST] = nullptr;
		children[SOUTH_WEST] = nullptr;
	}

	/* returns true if this node is a leaf */
	bool is_leaf()
	{
		return children[NORTH_EAST] == nullptr &&
			children[NORTH_WEST] == nullptr &&
			children[SOUTH_EAST] == nullptr &&
			children[SOUTH_WEST] == nullptr;
	}

	/* Getters */
	[[nodiscard]] Particle* get_particle() { return this->particle; }
	Quadrant get_quadrant() { return this->quadrant; }
	map<Ordinal, BHTNode*> get_children() { return this->children; }

	/* Setters */
	void set_particle(Particle* new_particle) { this->particle = new_particle; }

	/* puts the provided child node in the provided quadrant */
	void put_child(Ordinal ordinal, BHTNode* child) { this->children[ordinal] = child; }
};

