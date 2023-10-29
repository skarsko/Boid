#include <exception>
#include <stdexcept>
#include <iostream>
#include <vector>
#include "boid.hpp"
#include "param.hpp"


Boid::Boid(double x, double y, double vx, double vy){
    this->x = x;
    this->y = y;
    this->vx = vx;
    this->vy = vy;
}

Boid::~Boid(){
    //Nothing to do here yet
}

double Boid::getX(){
    return this->x;
}

double Boid::getY(){
    return this->y;
}

double Boid::getVx(){
    return this->vx;
}

double Boid::getVy(){
    return this->vy;
}

bool Boid::inBounds(){
    return (this->x >= 0 && this->x <= SCREEN_WIDTH && this->y >= 0 && this->y <= SCREEN_HEIGHT);
}

void Boid::setX(double x){
    this->x = x;
    this->periodicBoundaryConditions();
}

void Boid::setY(double y){
    this->y = y;
    this->periodicBoundaryConditions();
}

void Boid::setVx(double vx){
    this->vx = vx;
}

void Boid::setVy(double vy){
    this->vy = vy;
}


void Boid::periodicBoundaryConditions(){
   // Periodic boundary conditions
    if (this->x >= SCREEN_WIDTH) {
        this->x -= SCREEN_WIDTH;
    } else if (this->x < 0) {
        this->x += SCREEN_WIDTH;
    }
    if (this->y >= SCREEN_HEIGHT) {
        this->y -= SCREEN_HEIGHT;
    } else if (this->y < 0) {
        this->y += SCREEN_HEIGHT;
    }
}



void Boid::updatePosition(double dt){
    // We check if any other boids are too close to the boid
    this->setX(this->x + this->vx * dt);
    this->setY(this->y + this->vy * dt);
}