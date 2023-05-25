#pragma once

#ifndef BHTREE_H
#define BHTREE_H

#include <cmath>
#include <map>

#include "BHTNode.hpp"

/* implementation of the Barnes-Hut quad-tree */
class BHT
{
private:
	BHTNode* root;
	int size;

	/* helper for insert, calls it for the appropriate quadrant */
	void insert_in_quadrant(Particle* particle, BHTNode* node)
	{
		// determine which quadrant the node is in, then calls insert
		map<Ordinal, BHTNode*> children = node->get_children();
		for (auto& [ordinal, child] : children)
		{
			if (child != nullptr && particle->is_in_quadrant(child->get_quadrant()))
			{
				insert(particle, child);
				return;
			}
		}
	}

	/* recursive helper for the insert function, recurse until we find
	 * a valid empty Node to store this Particle */
	void insert(Particle* particle, BHTNode* node)
	{
		Quadrant node_quadrant = node->get_quadrant();

		// if the last node was a leaf
		if (node == nullptr) return;

		// if the current Node is empty, insert the particle inside it
		if (Particle* node_particle = node->get_particle(); node_particle == nullptr)
		{
			node->set_particle(particle);
		}
		else if (node->is_leaf()) // if the Node is a leaf
		{
			// subdivide the region further by creating four children
			map<Ordinal, Quadrant*> sub_quadrants = node_quadrant.subdivide();

			for (auto& [ordinal, quadrant] : sub_quadrants)
			{
				node->put_child(ordinal, new BHTNode(*quadrant));
			}

			// since we subdivided the quadrant in four, we insert both the new particle
			// and the one that was in the bigger quadrant into the new quadrants
			insert_in_quadrant(node->get_particle(), node);
			insert_in_quadrant(particle, node);

			// update the node's center of mass and total mass
			node->set_particle(node_particle->approx_COM(particle));
		}
		else // if the Node is internal
		{
			// update its center of mass and total mass
			node->set_particle(node_particle->approx_COM(particle));

			// insert the particle into the appropriate quadrant
			insert_in_quadrant(particle, node);
		}
	}

	/* recursive helper for the apply_forces function computes and apply forces on the Particle */
	void apply_forces(Particle* particle, BHTNode* node)
	{
		Particle* node_particle = node->get_particle();

		// only apply forces if there is a particle
		// empty nodes don't have children
		// particles don't exert forces on themselves
		if (particle == nullptr || node_particle == nullptr || particle == node_particle ) return;

		// if the node is a leaf, compute the force it exerts on the particle and apply it
		if (node->is_leaf()) 
		{
			particle->apply_force(node_particle);
		}
		else // if this is an internal node 
		{
			// length of a side of the quadrant the node is in
			double length = node->get_quadrant().get_length();

			// euclidean distance between this node's center-of-mass and the particle
			double distance = particle->get_dist(node_particle);

			// if the node is far enough from this particle, compute the force its
			// subtree exerts on the particle and apply it
			if (length/distance < theta)
			{
				particle->apply_force(node_particle);
			}
			else // visit the node's children
			{
				apply_forces(particle, node->get_children()[NORTH_EAST]);
				apply_forces(particle, node->get_children()[NORTH_WEST]);
				apply_forces(particle, node->get_children()[SOUTH_EAST]);
				apply_forces(particle, node->get_children()[SOUTH_WEST]);
			}
		}
	}

	/* recursive helper to render the nodes' Particles and Quadrants */
	void display(BHTNode* node)
	{
		// if the last node was a leaf
		if (node == nullptr) return;

		// display the node's Particle
		Particle* particle = node->get_particle();
		if (particle != nullptr) particle->display();

		// display the node's Quadrant iff the feature is toggled on
		if (render_quadrants)
		{
			Quadrant quadrant = node->get_quadrant();
			quadrant.display();
		}

		// visit the node's children
		if (!node->is_leaf())
		{
			display(node->get_children()[NORTH_EAST]);
			display(node->get_children()[NORTH_WEST]);
			display(node->get_children()[SOUTH_EAST]);
			display(node->get_children()[SOUTH_WEST]);
		}
	}

public:
	/* constructor */
	explicit BHT(BHTNode* root) : root(root)
	{
		this->size = (root == nullptr) ? 0 : 1;
	}

	/* getters */
	[[nodiscard]] int get_size() { return this->size; }
	[[nodiscard]] BHTNode* get_root() { return this->root; }

	/* insert the provided Particle into this Barnes-Hut tree */
	void insert(Particle* particle)
	{
		insert(particle, root);
		size++;
	}
	
	/* computes and applies all the external forces exerted
	 * on the provided Particle */
	void apply_forces(Particle* particle)
	{
		apply_forces(particle, root);
	}

	/* displays the clusters */
	void display()
	{
		display(root);
	}

};

#endif