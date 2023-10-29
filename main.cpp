#include <SDL2/SDL.h>
#include <vector>
#include <random>
#include <ctime>
#include <iostream>
#include <math.h>
#include <cmath>
#include "src/param.hpp"
#include "src/boid.hpp"
#include "src/generateBoids.hpp"
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 960;
const int BOID_RADIUS = 5;

//Predeclaration of helper functions
void separate(std::vector<Boid> &boids, double separationDistance, double avoidFactor, double maxSpeed);
void align(std::vector<Boid> &boids, double alignmentDistance, double alignmentFactor, double maxSpeed);
void cohere(std::vector<Boid> &boids, double cohesionDistance, double cohesionFactor, double maxSpeed);
void avoidBoundary(std::vector<Boid> &boids, double turnFactor, double margin);

int main(int argc, char* argv[]) {
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);


    // Create a window
    SDL_Window* window = SDL_CreateWindow("Particle Animation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    // Create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    //Seeds the rand()
    srand(static_cast<unsigned int>(time(nullptr)));

    //Gnerates boids randomly
    int numBoids = 100;
    double maxSpeed = 6;
    std::vector<Boid> boids = generateBoidsInCircle(numBoids, maxSpeed);

    // Start the game loop
    bool quit = false;
    while (!quit) {
        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // Quit if the user presses the close button 
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }       
        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 181, 232, 179, 255);
        SDL_RenderClear(renderer);
        // Draw the boids
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        for(int i = 0; i < boids.size(); i++){
            for(int j = boids[i].getX()-BOID_RADIUS; j < boids[i].getX()+BOID_RADIUS; j++){
                //Drawing them as circles of radius BOID_RADIUS
                for(int k = boids[i].getY()-BOID_RADIUS; k < boids[i].getY()+BOID_RADIUS; k++){
                    if(pow(j-boids[i].getX(), 2) + pow(k-boids[i].getY(), 2) <= pow(BOID_RADIUS, 2)){
                        SDL_RenderDrawPoint(renderer, j, k);
                    }
                }
            }
        }
        
        // Update the boids
        double dt = .1;
        for(Boid &b : boids){
            b.updatePosition(dt);
        }

        // Add repulsive force to boids which are too close
        int separationDistance = 50;
        double avoidFactor = 0.00008;
        separate(boids, separationDistance, avoidFactor, maxSpeed);

        //Add alignment force to boids which are close
        int alignmentDistance = 100;
        double alignmentFactor = 0.002;
        align(boids, alignmentDistance, alignmentFactor, maxSpeed);

        //Add cohesion force to boids which are somewhat close
        int cohesionDistance = 200;
        double cohesionFactor = 0.0001;
        cohere(boids, cohesionDistance, cohesionFactor, maxSpeed);

        //Add force to boids which are close to the boundary
        double turnFactor = 0.05;
        double margin = 0.1;
        avoidBoundary(boids, turnFactor, margin);

        // Delay and update the screen
        int simulationDelay = 0;
        SDL_Delay(simulationDelay);
        SDL_RenderPresent(renderer);
    }
    

/////////////////////////////////////////////////////////////////////////

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


//Helper functions
void separate(std::vector<Boid> &boids, double separationDistance, double avoidFactor, double maxSpeed){
   for(Boid &boid : boids){
    double dxCorrective = 0;
    double dyCorrective = 0;
    //Every boid which is too close to our boid contributes to the corrective force
    for(Boid &otherBoid : boids){
        if(&boid != &otherBoid){
            if(pow(boid.getX()-otherBoid.getX(), 2) + pow(boid.getY()-otherBoid.getY(), 2) <= pow(separationDistance, 2)){
                dxCorrective += boid.getX()-otherBoid.getX();
                dyCorrective += boid.getY()-otherBoid.getY();
            }
        }
    }
    //Now the boids velocity is updatet with the corrective force and some avoidness-factor
    boid.setVx(boid.getVx() + avoidFactor * dxCorrective);
    boid.setVy(boid.getVy() + avoidFactor * dyCorrective);

    //We also want to make sure that the boids don't exceed the maximum speed
    double speed = sqrt(pow(boid.getVx(), 2) + pow(boid.getVy(), 2));
    if(speed > maxSpeed){
        boid.setVx(boid.getVx() * maxSpeed/speed);
        boid.setVy(boid.getVy() * maxSpeed/speed);
    }
   }
}

void align(std::vector<Boid> &boids, double alignmentDistance, double alignmentFactor, double maxSpeed){
    for(Boid &boid : boids){
        double VxCorrective = 0;
        double VyCorrective = 0;
        int neighbors = 0;
        for(Boid &otherBoid : boids){
            if(&boid != &otherBoid){
                if(pow(boid.getX()-otherBoid.getX(), 2) + pow(boid.getY()-otherBoid.getY(), 2) <= pow(alignmentDistance, 2)){
                    //Add corrective force if other boid is close
                    VxCorrective += otherBoid.getVx();
                    VyCorrective += otherBoid.getVy();
                    neighbors++;
                }
            }
        }
        //Now the boids velocity is updatet with the corrective force and some avoidness-factor
        if(neighbors > 0){
            boid.setVx(boid.getVx() + alignmentFactor * VxCorrective/neighbors);
            boid.setVy(boid.getVy() + alignmentFactor * VyCorrective/neighbors);
        }
    }
}

void cohere(std::vector<Boid> &boids, double cohesionDistance, double cohesionFactor, double maxSpeed){
    for(Boid &boid : boids){
        double centerOfMassX = 0;
        double centerOfMassY = 0;
        int neighbors = 0;
        for(Boid &otherboid : boids){
            if(&boid != &otherboid){
                if(pow(boid.getX()-otherboid.getX(), 2) + pow(boid.getY()-otherboid.getY(), 2) <= pow(cohesionDistance, 2)){
                    //Add corrective force if other boid is close
                    centerOfMassX += otherboid.getX();
                    centerOfMassY += otherboid.getY();
                    neighbors++;
                }
            }
        }
        //Now we update the boids velocity to move a bit towards the center of mass
        if(neighbors > 0){
            boid.setVx(boid.getVx() + cohesionFactor * (centerOfMassX/neighbors - boid.getX()));
            boid.setVy(boid.getVy() + cohesionFactor * (centerOfMassY/neighbors - boid.getY()));
        }
    }
}

void avoidBoundary(std::vector<Boid> &boids, double turnFactor, double margin){
    for(Boid &boid : boids){
        if(boid.getX() < margin*SCREEN_WIDTH){
            boid.setVx(boid.getVx() + turnFactor);
        }
        if(boid.getX() > SCREEN_WIDTH-margin*SCREEN_WIDTH){
            boid.setVx(boid.getVx() - turnFactor);
        }
        if(boid.getY() < margin*SCREEN_HEIGHT){
            boid.setVy(boid.getVy() + turnFactor);
        }
        if(boid.getY() > SCREEN_HEIGHT-margin*SCREEN_HEIGHT){
            boid.setVy(boid.getVy() - turnFactor);
        }
    }
}