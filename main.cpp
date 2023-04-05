#include <SDL2/SDL.h>
#include <vector>
#include <random>
#include <ctime>
#include <iostream>
#include <math.h>
#include <cmath>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 960;
const int PARTICLE_COUNT = 300;
const int delay = 0; //in ms
const int particleRadius = 5;
const int PARTICLE_MASS = 3;
const int REPULSION_THRESHOLD = 7 * particleRadius;
const float FORCE = 50.0f;
const float INITIAL_VELOCITY = .2;
const bool DRAW_RAY = false;
double RAY_LENGTH = 100;
const double RAY_WIDTH = M_PI / 4;

struct Particle {
    double x, y, dx, dy;
    double angle;
    Uint8 r, g, b;
    Uint8 radius;
    bool directionTendency;
    int lastChange;
};

int main(int argc, char* argv[]) {
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);


    // Create a window
    SDL_Window* window = SDL_CreateWindow("Particle Animation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    // Create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    //Seeds the rand()
    srand(static_cast<unsigned int>(time(nullptr)));

    // Generate particles
    std::vector<Particle> particles(PARTICLE_COUNT);
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(0, 2*M_PI);
    double xCut = 2.0*M_PI/particles.size();
    int counter = 0;
    for (auto& particle : particles) {
        auto middleRad = 50;
        particle.x = SCREEN_WIDTH/2 + 100*sin(counter*xCut);
        particle.y = SCREEN_HEIGHT/2 + 100*cos(counter*xCut);
        particle.radius = particleRadius;
        double randomAngle = distribution(generator);
        particle.angle = randomAngle;
        particle.dx = cos(randomAngle)*INITIAL_VELOCITY;
        particle.dy = sin(randomAngle)*INITIAL_VELOCITY;
        particle.r = 0; 
        particle.g = 0;
        particle.b = 0;
        particle.directionTendency = rand()%2;
        particle.lastChange = 300;
        counter++;
    }

    // Start the game loop
    bool quit = false;
    while (!quit) {
        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }



        


        // Update particles
        for (auto& particle : particles) {      
            particle.angle = std::fmod(particle.angle, 2*M_PI);
            particle.x += cos(particle.angle)*INITIAL_VELOCITY;
            particle.y += sin(particle.angle)*INITIAL_VELOCITY;
            
            enum side{
                up, down, left, right
            };

            double radius = particle.radius;
            double distanceLeftWall = particle.x - radius;
            double distanceRightWall = SCREEN_WIDTH - particle.x - radius;
            double distanceUpperWall = particle.y - radius;
            double distanceLowerWall = SCREEN_HEIGHT - particle.y - radius;
            double nearestDistance = std::min(distanceLeftWall,distanceRightWall,distanceUpperWall,distanceLowerWall);
            side nearestWall;
            if(nearestDistance == distanceLeftWall){
                nearestWall = left;
            }else
            if(nearestDistance = distanceRightWall){
                nearestWall = right;
            }

        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 181, 232, 179, 255);
        SDL_RenderClear(renderer);


//////////////////////////////////////////////////////////////////////////////////////////
    // Draw particles
        for (auto& particle : particles) {
            SDL_SetRenderDrawColor(renderer, particle.r, particle.g, particle.b, 255);
            SDL_RenderDrawPoint(renderer, particle.x, particle.y);
            for(int i = -particle.radius; i < particle.radius; i++){
                for(int j = -particle.radius; j < particle.radius; j++){
                    if(sqrt(i*i+j*j)<particle.radius){
                    SDL_RenderDrawPoint(renderer, particle.x+i, particle.y+j);
                    }
                }   
            }

        //Draw the rays
         SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            if(DRAW_RAY){
                double initialX = particle.x;
                double initialY = particle.y;
                double rayLength = 100 * 1/INITIAL_VELOCITY;
                double alphaDiff = RAY_WIDTH;
                double oldAlpha = particle.angle;
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderDrawLine(renderer, initialX, initialY, initialX + rayLength * cos(oldAlpha+alphaDiff) * INITIAL_VELOCITY, initialY + rayLength * sin(oldAlpha+alphaDiff) * INITIAL_VELOCITY);
                SDL_RenderDrawLine(renderer, initialX, initialY, initialX + rayLength * cos(oldAlpha-alphaDiff) * INITIAL_VELOCITY, initialY + rayLength * sin(oldAlpha-alphaDiff) * INITIAL_VELOCITY);
            }            
        
        }
        
        SDL_Delay(delay);
        // Update the screen
        SDL_RenderPresent(renderer);
    }

/////////////////////////////////////////////////////////////////////////

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
