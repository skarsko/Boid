#include <SDL2/SDL.h>
#include <vector>
#include <random>
#include <ctime>
#include <iostream>
#include <math.h>
#include <cmath>
#include <SDL2/SDL_ttf.h>
#include <chrono>
#include <algorithm>
#include "src/param.hpp"
#include "src/boid.hpp"
#include "src/generateBoids.hpp"
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int BOID_RADIUS = 5;

//Main-only parameters
int numBoids = 550;
double maxSpeed = 6;
int separationDistance = 50;
double avoidFactor = 0.00008;
int alignmentDistance = 100;
double alignmentFactor = 0.002;
int cohesionDistance = 200;
double cohesionFactor = 0.0001;
double turnFactor = 0.05;
double margin = 0.1;
double predatorAvoidDistance = 150;
double predatorStrength = 0.01;
bool predator = false;
double avoidMeanGroupDistance = 400;
//Set these to 0 to disable hunting/being afraid of mean group 0
double avoidMeanGroupFactor = 0.0000005;
double huntFactor = 0.0001;

//Predeclaration of helper functions
void separate(std::vector<Boid> &boids, double separationDistance, double avoidFactor, double maxSpeed);
void align(std::vector<Boid> &boids, double alignmentDistance, double alignmentFactor, double maxSpeed);
void cohere(std::vector<Boid> &boids, double cohesionDistance, double cohesionFactor, double maxSpeed);
void avoidBoundary(std::vector<Boid> &boids, double turnFactor, double margin);
void drawText (SDL_Surface* screen, char* string, int size, int x, int y, int fR, int fG, int fB, int bR, int bG, int bB);
void avoidPredator(std::vector<Boid> &boids, double predatorX, double predatorY, double avoidDistance, double strength);
void avoidMeanGroup(std::vector<Boid> &boids, double avoidDistance, double avoidFactor);
void hunt(std::vector<Boid> &boids, double huntFactor);




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
    std::vector<Boid> boids = generateBoidsRandomly(numBoids, maxSpeed);

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
            // If player presses the left mouse button, toggle predator mode
            if(event.type == SDL_MOUSEBUTTONDOWN){
                if(event.button.button == SDL_BUTTON_LEFT){
                    predator = !predator;
                }
            }
        }       
        // Clear the screen + black background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

    
        // Draw the boids
        for(int i = 0; i < boids.size(); i++){
            //Color depends on which group the boid is in
            switch (boids[i].getGroup()){
                case 0:
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); //mean group (red)
                    break;
                case 1:
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                    break;
                case 2:
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                    break;
                case 3:
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                    break;
                default: 
                    throw std::invalid_argument("Invalid group number");
            }

            //Draw the Boids with two lines like birds
            double angle = atan2(boids[i].getVy(), boids[i].getVx());
            int x1 = boids[i].getX() + BOID_RADIUS * cos(angle);
            int y1 = boids[i].getY() + BOID_RADIUS * sin(angle);
            int x2 = boids[i].getX() + BOID_RADIUS * cos(angle + 2.35619);
            int y2 = boids[i].getY() + BOID_RADIUS * sin(angle + 2.35619);
            int x3 = boids[i].getX() + BOID_RADIUS * cos(angle - 2.35619);
            int y3 = boids[i].getY() + BOID_RADIUS * sin(angle - 2.35619);
            SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
            SDL_RenderDrawLine(renderer, x1, y1, x3, y3);

            //Drawing them as circles of radius BOID_RADIUS
            /*for(int j = boids[i].getX()-BOID_RADIUS; j < boids[i].getX()+BOID_RADIUS; j++){          
                for(int k = boids[i].getY()-BOID_RADIUS; k < boids[i].getY()+BOID_RADIUS; k++){
                    if(pow(j-boids[i].getX(), 2) + pow(k-boids[i].getY(), 2) <= pow(BOID_RADIUS, 2)){
                        SDL_RenderDrawPoint(renderer, j, k);
                    }
                }
            }*/
        }
        
       
        // Update the boids
        double dt = .1;
        for(Boid &b : boids){
            b.updatePosition(dt);
        }
        

        // Add repulsive force to boids which are too close
        separate(boids, separationDistance, avoidFactor, maxSpeed);

        //Add alignment force to boids which are close   
        align(boids, alignmentDistance, alignmentFactor, maxSpeed);

        //Add cohesion force to boids which are somewhat close     
        cohere(boids, cohesionDistance, cohesionFactor, maxSpeed);

        //Add force to boids which are close to the boundary    
        avoidBoundary(boids, turnFactor, margin);   

        //Let the boid avoid the mean group 0
        avoidMeanGroup(boids, avoidMeanGroupDistance, avoidMeanGroupFactor);

        //Let mean group 0 hunt the other groups
        hunt(boids, huntFactor);

        if(predator){
            //Get mouse position
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            //Add force to boids which are close to the mouse
            avoidPredator(boids, mouseX, mouseY, predatorAvoidDistance, predatorStrength);
            //Draw mean red circle around mouse
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            int predatorRadius = 10;
            for(int i = mouseX-predatorRadius; i < mouseX+predatorRadius; i++){
                for(int j = mouseY-predatorRadius; j < mouseY+predatorRadius; j++){
                    if(pow(i-mouseX, 2) + pow(j-mouseY, 2) <= pow(predatorRadius, 2)){
                        SDL_RenderDrawPoint(renderer, i, j);
                    }
                }
            }
        }

      

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
            if(&boid != &otherBoid && boid.getGroup() == otherBoid.getGroup()){
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
            if(&boid != &otherboid && boid.getGroup() == otherboid.getGroup()){
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

void avoidPredator(std::vector<Boid> &boids, double predatorX, double predatorY, double avoidDistance, double strength){
    for(Boid &boid : boids){
        if(boid.getGroup() != 0 && pow(boid.getX()-predatorX, 2) + pow(boid.getY()-predatorY, 2) <= pow(avoidDistance, 2)){
            boid.setVx(boid.getVx() + strength * (boid.getX()-predatorX));
            boid.setVy(boid.getVy() + strength * (boid.getY()-predatorY));  
        }
    }
}

void avoidMeanGroup(std::vector<Boid> &boids, double avoidDistance, double avoidFactor){
    for(Boid &boid : boids){
        if(boid.getGroup() != 0){
            for(Boid &otherBoid : boids){
                if(otherBoid.getGroup() == 0){
                    double distance = pow(boid.getX()-otherBoid.getX(), 2) + pow(boid.getY()-otherBoid.getY(), 2);
                    if(distance <= pow(avoidDistance, 2)){
                        boid.setVx(boid.getVx() + avoidFactor * (boid.getX()-otherBoid.getX()));
                        boid.setVy(boid.getVy() + avoidFactor * (boid.getY()-otherBoid.getY()));
                    }
                }
            }
        }
    }
}

void hunt(std::vector<Boid> &boids, double huntFactor){
    //Get the mean group to the center of mass
    double centerOfMassX = 0;
    double centerOfMassY = 0;
    int numBoids = boids.size();
    for(Boid &boid : boids){
        centerOfMassX += boid.getX();
        centerOfMassY += boid.getY();
    }
    centerOfMassX /= numBoids;
    centerOfMassY /= numBoids;
    for(Boid &boid : boids){
        if(boid.getGroup()==0){
            boid.setVx(boid.getVx() + huntFactor * (centerOfMassX - boid.getX()));
            boid.setVy(boid.getVy() + huntFactor * (centerOfMassY - boid.getY()));
        }
    }
}