#include "boid.hpp"
#include "param.hpp"
#include <vector>
#include <random>
#include <cmath>

void setRandomVelocities(std::vector<Boid> &boids, double maxSpeed);

std::vector<Boid> generateBoidsRandomly(int numBoids, double maxSpeed){
    std::vector<Boid> boids;
    for(int i = 0; i < numBoids; i++){
        double x = 5 + rand() % (SCREEN_WIDTH-10);
        double y = 5 + rand() % (SCREEN_HEIGHT-10);
        boids.push_back(Boid(x, y, 0, 0));
    }
    setRandomVelocities(boids, maxSpeed);
    return boids;
}

std::vector<Boid> generateBoidsInHLine(int numBoids, double maxSpeed){
    std::vector<Boid> boids;
    for(int i = 0; i < numBoids; i++){
        double x = 5 + rand() % (SCREEN_WIDTH-10);
        double y = SCREEN_HEIGHT/2;
        boids.push_back(Boid(x, y, 0, 0));
    }
    setRandomVelocities(boids, maxSpeed);
    return boids;
}

std::vector<Boid> generateBoidsInVLine(int numBoids, double maxSpeed){
    std::vector<Boid> boids;
    for(int i = 0; i < numBoids; i++){
        double x = SCREEN_WIDTH/2;
        double y = 5 + rand() % (SCREEN_HEIGHT-10);
        boids.push_back(Boid(x, y, 0, 0));
    }
    setRandomVelocities(boids, maxSpeed);
    return boids;
}

std::vector<Boid> generateBoidsInCircle(int numBoids, double maxSpeed){
    std::vector<Boid> boids;
    double pi_d = 2*M_PI/numBoids;
    for(int i = 0; i < numBoids; i++){
        double x = SCREEN_WIDTH/2 + (SCREEN_WIDTH/2 - 10) * cos(pi_d * i);
        double y = SCREEN_HEIGHT/2 + (SCREEN_HEIGHT/2 - 10) * sin(pi_d * i);
        boids.push_back(Boid(x, y, 0, 0));
    }
    setRandomVelocities(boids, maxSpeed);
    return boids;
}

void setRandomVelocities(std::vector<Boid> &boids, double maxSpeed){
    for(int i = 0; i < boids.size(); i++){
        double vx = rand() * maxSpeed/RAND_MAX;
        double vy = rand() * maxSpeed/RAND_MAX;
        if(rand() % 2 == 0){
            vx *= -1;
        }
        if(rand() % 2 == 0){
            vy *= -1;
        }
        boids[i].setVx(vx);
        boids[i].setVy(vy);
    }
}