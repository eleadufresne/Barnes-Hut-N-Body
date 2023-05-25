#pragma once

#ifndef CONFIG_H
#define CONFIG_H

// CHANGE THIS FOR KEYBOARD INPUT
inline bool custom_input = false; // this forces the scale to be smaller

/* parameters for the simulation */

// initial conditions 
inline int N = 700; // number of bodies
inline double radius = 2.5e05; // length the largest quadrant's side in real-life scale
inline double min_mass = 1.5e14;
inline double max_mass = 1.5e16;
inline double min_velocity = -1500.0;
inline double max_velocity = 1500.0;
double avg_mass;

// constants used in computations
constexpr double theta = 1.25; // threshold of ~1 (opening angle) https://sci-hub.se/https://www.nature.com/articles/324446a0
constexpr double dt = 0.25; // time step
constexpr double eps = 25000; // softening parameter
constexpr double G = 6.6743e-11; // gravitational constant

// variables for display
constexpr int window_height = 720 * 2; // 1280; // 720; //
constexpr int window_width = 720 * 2; // window_height; // 
inline bool render_quadrants = true; // true if we want to display quadrants
inline double scale = 0.5 / radius; // scale from irl coordinates to window coordinates

#endif
