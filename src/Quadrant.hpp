#pragma once

#include <map>
#include <GL/glew.h>
#include <glm/vec2.hpp>

#include "config.h"
#include "ordinal.h"

using namespace std;

/* bounding squared region */
class Quadrant
{
private:
	glm::dvec2 centre_; // coordinates of the centre
	double length_; // length of one side 

public:
	/* constructor */
	Quadrant(glm::dvec2 centre, double length)
		: centre_(centre),
		length_(length)
	{
	}

	/* getter */
	[[nodiscard]] double get_length() { return this->length_; }

	/* return true if the given point is inside this Quadrant */
	[[nodiscard]] bool contains(glm::dvec2 point)
	{
		// half this region's side length
		double half_length = length_ / 2.0;
		// compute the bounds of the region
		auto lower_bound = glm::dvec2(centre_.x - half_length, centre_.y - half_length);
		auto upper_bound = glm::dvec2(centre_.x + half_length, centre_.y + half_length);
		// check if the point is inside the bounds 
		return lower_bound.x <= point.x && point.x <= upper_bound.x
			&& lower_bound.y <= point.y && point.y <= upper_bound.y;
	}


	/* divides this Quadrant into four new sub-quadrants */
	[[nodiscard]] map<Ordinal, Quadrant*> subdivide()
	{
		map<Ordinal, Quadrant*> quadrants;

		// quarter of this region's side length
		double half_length = length_ / 2.0;
		double quarter_length = half_length / 2.0;

		// compute the new centre using the quarter length
		auto northeast_centre = glm::dvec2(centre_.x + quarter_length, centre_.y + quarter_length);
		auto northwest_centre = glm::dvec2(centre_.x - quarter_length, centre_.y + quarter_length);
		auto southeast_centre = glm::dvec2(centre_.x + quarter_length, centre_.y - quarter_length);
		auto southwest_centre = glm::dvec2(centre_.x - quarter_length, centre_.y - quarter_length);

		// return the new quadrants; the new length of a side is the current half length
		quadrants[NORTH_EAST] = new Quadrant(northeast_centre, half_length);
		quadrants[NORTH_WEST] = new Quadrant(northwest_centre, half_length);
		quadrants[SOUTH_EAST] = new Quadrant(southeast_centre, half_length);
		quadrants[SOUTH_WEST] = new Quadrant(southwest_centre, half_length);

		return quadrants;
	}

	/* displays this quadrant */
	void display()
	{
		// half the length of the side
		double half_length = length_ / 2.0;

		// extreme coordinates (scaled to the simulation)
		double min_x = (centre_.x - half_length) * scale;
		double max_x = (centre_.x + half_length) * scale;
		double min_y = (centre_.y + half_length) * scale;
		double max_y = (centre_.y - half_length) * scale;

		// render the quadrants 
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBegin(GL_LINE_LOOP);

		glColor4d(0.0, 250.0, 0.0, 0.25);
		glVertex2d(min_x, min_y);
		glVertex2d(max_x, min_y);
		glVertex2d(max_x, max_y);
		glVertex2d(min_x, max_y);

		glEnd();
	}
};
