#pragma once
#include <GL/glew.h>
#include <glm/vec2.hpp>

#include "config.h"
using namespace std;

/* particle that is stored in a BH tree node */
class Particle
{
private:

	double mass;
	glm::dvec2 position; // coordinates of the centre of mass
	glm::dvec2 velocity;
	glm::dvec2 force; // to compute the acceleration

public:
	/* constructor */
	Particle(double mass, glm::dvec2 position, glm::dvec2 velocity, glm::dvec2 force)
		: mass(mass),
		position(position),
		velocity(velocity),
		force(force)
	{
	}

	/* getters */
	double get_mass() { return this->mass; }
	glm::dvec2 get_position() { return this->position; }
	glm::dvec2 get_velocity() { return this->velocity; }
	glm::dvec2 get_force() { return this->force; }

	/* setters */
	void set_velocity(glm::dvec2 new_velocity) { this->velocity = new_velocity; }
	void set_position(glm::dvec2 new_position) { this->position = new_position; }

	/* resets the force */
	void reset_force() { force = glm::dvec2(0.0, 0.0); }

	/* returns true if this particle is inside the given quadrant */
	[[nodiscard]] bool is_in_quadrant(Quadrant quadrant)
	{
		return quadrant.contains(this->position);
	}

	/* approximates the center-of-mass of the this Particle with the provided one */
	Particle* approx_COM(Particle* particle)
	{
		// if no particle was provided, do nothing
		if (particle == nullptr) return this;
		// step the mass, position, and velocities
		double new_mass = this->mass + particle->mass;
		glm::dvec2 new_position = (this->position * this->mass + particle->position * particle->mass) / new_mass;
		glm::dvec2 new_velocity = (this->velocity * this->mass + particle->velocity * particle->mass) / new_mass;
		// return a particle that is at the COM
		return new Particle(new_mass, new_position, new_velocity, glm::dvec2(0.0, 0.0));
	}

	/* updates the force exerted on this particle */
	void apply_force(Particle* particle)
	{
		// if no particle was provided, do nothing
		if (particle == nullptr) return;
		
		// compute the gravitational force between the particles
		double distance = get_dist(particle);
		double grav = G * (this->mass * particle->get_mass()) / pow(distance*distance + eps*eps, 1.5);

		// update this particle's force
		this->force += grav * (particle->get_position()-this->position);
	}

	glm::dvec2 initial_velocity, initial_position;

	/* updates the velocity and position of this particle
	 * https://medium.com/swlh/create-your-own-n-body-simulation-with-python-f417234885e9 */
	void step(double dt)
	{
		// save the initial velocity and position (useful for half kicks and full drifts)
		initial_velocity = velocity;
		initial_position = position;
		// half kick 
		velocity += (force / mass) * dt * 0.5;
		// full drift
		position += velocity * dt;
	}

	/* returns the distance between this Particle's COM and the provided one's */
	double get_dist(Particle* particle)
	{
		double dx = position.x - particle->position.x;
		double dy = position.y - particle->position.y;
		return sqrt(dx * dx + dy * dy);
	}

	/* displays this particle */
	void display()
	{
		glPointSize(3);
		glBegin(GL_POINTS);

		glColor3d(245, 137, 162);
		glVertex2d(this->position.x * scale, this->position.y * scale);

		glEnd();
	}
};