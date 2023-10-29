#include "boid.hpp"

//Generates Boids randomly on the x and y axis
std::vector<Boid> generateBoidsRandomly(int numBoids, double maxSpeed);
//Generates Boids randomly in a line (which is the x-axis)
std::vector<Boid> generateBoidsInHLine(int numBoids, double maxSpeed);
//Generates Boids randomly in a line (which is the y-axis)
std::vector<Boid> generateBoidsInVLine(int numBoids, double maxSpeed);
//Generates Boids randomly in a circle, where the circle is not necaissary a circle
//but eventually an elipse, if the screen is not a square. 
std::vector<Boid> generateBoidsInCircle(int numBoids, double maxSpeed);