#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <iostream>
#include <random>
#include <cmath>

#include "BHT.hpp"
#include <list>
#include <thread>

/* variables */
list<Particle*> particles;
bool paused = false;

/* randomly generated initial cluster */
list<Particle*>& init_particles()
{
	// reset the particles
	particles.clear();
	glm::dvec2 force(0.0, 0.0);

	if( custom_input )
	{
		double x_coordinate, y_coordinate, x_velocity, y_velocity, mass;
		int r, g, b; // we won't use these
		particles.clear();

		// first set the number of bodies and the radius
		cin >> N >> radius;

		// for each particle
		for (int i = 0; i < N; ++i)
		{
			// save the coordinates, velocities, and masses
			cin >> mass >> x_coordinate >> y_coordinate >> x_velocity >> y_velocity;

			// add the new Particle to the list
			auto particle = new Particle(mass, glm::dvec2(x_coordinate, y_coordinate), glm::dvec2(x_velocity, y_velocity), force);
			particles.push_back(particle);
		}

		// scale for the simulation
		scale = 0.1 / radius;
	}
	else
	{
		// random generators
		mt19937_64 rng(radius * 2.0);
		uniform_real_distribution<double> dist(-1.0, 1.0);

		// sun
		auto sun = new Particle(1.25 * max_mass, glm::dvec2(0.0, 0.0), glm::dvec2(max_velocity, min_velocity), glm::dvec2(0.0, 0.0));
		particles.push_back(sun);

		/* random = offset + (rand() % range) */
		for (int i = 1; i < N; ++i)
		{
			// initial mass
			double mass = static_cast<double>(rand()) / RAND_MAX * (max_mass - min_mass) + min_mass;

			// initial position
			double r = pow(pow(1.0 / (dist(rng) * dist(rng) + 1e-7), 2) - 1, -0.5);
			double phi = dist(rng) * 2 * M_PI;
			glm::dvec2 position(r * cos(phi) * radius, r * sin(phi) * radius);

			// initial velocity
			glm::dvec2 velocity(static_cast<double>(rand()) / RAND_MAX * (max_velocity - min_velocity) + min_velocity,
				static_cast<double>(rand()) / RAND_MAX * (max_velocity - min_velocity) + min_velocity);

			// save the Particle (initially it has no force)
			auto particle = new Particle(mass, position, velocity, glm::dvec2(0.0, 0.0));
			particles.push_back(particle);
		}
	}
	return particles;
}

/* called when the window is resized */
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

/* called when a key is pressed */
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if ((action != GLFW_PRESS) && (action != GLFW_REPEAT)) return;
	if (key == GLFW_KEY_ESCAPE)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	else if (key == GLFW_KEY_SPACE)
	{
		paused = !paused;
		if (paused) cout << "Simulation paused." << endl;
		else cout << "Simulation resumed." << endl;
	}
	else if (key == GLFW_KEY_R)
	{
		cout << "Simulation reinitialized." << endl;
		particles = init_particles();
	}
	else if (key == GLFW_KEY_Q)
	{
		render_quadrants = !render_quadrants;
		cout << "Toggled quadrant rendering." << endl;
	}
}


int main()
{
	// initialize GLFW
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	// create a windowed mode window and its OpenGL context
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Barnes-Hut N-Body Simulation", nullptr,
		nullptr);
	if (!window)
	{
		std::cerr << "Failed to create a window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// make the window's context current
	glfwMakeContextCurrent(window);

	// initialize GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		glfwTerminate();
		return -1;
	}
	glGetError(); // A bug in glewInit() causes an error that we can safely ignore.
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	// set interaction callbacks
	glfwSetKeyCallback(window, key_callback);

	// set the viewport size
	glViewport(0, 0, window_width, window_height);

	// register frame buffer size callback
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	// initialize the simulation
	particles = init_particles();
	Quadrant universe(glm::dvec2(0.0, 0.0), radius * 4);

	// main loop, runs until the window is closed
	while (!glfwWindowShouldClose(window))
	{
		// render the background and particles
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		// construct a Barnes-Hut tree
		BHT bht(new BHTNode(universe));
		for (auto particle : particles) 
			if (particle->is_in_quadrant(universe)) 
				bht.insert(particle);
		
		bht.display();

		// leapfrog integration : https://academic.oup.com/pasj/article/59/6/1095/1513697
		// compute the forces at time t 
		for (auto particle : particles)
		{
			particle->reset_force();
			bht.apply_forces(particle);
		}
		// step the velocity (half kick) and step the position using the new velocity (full drift) 
		for (auto particle : particles)
			particle->step(dt);
		// calculate the force at t + dt using the new positions
		for (auto particle : particles) {
			bht.apply_forces(particle);
		}
		// step the velocity again (using the initial velocity)
		for (auto particle : particles)
		{
			particle->set_velocity(particle->initial_velocity);
			particle->step(dt);
			particle->set_position(particle->initial_position);
		}

		// swap front and back buffers
		glfwSwapBuffers(window);

		// poll for and process events
		glfwPollEvents();

		// wait while the simulation is paused
		while (paused)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			glfwPollEvents();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		// free memory
		delete bht.get_root();
	}

	// quit program
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}


