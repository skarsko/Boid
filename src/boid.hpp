#ifndef BOID_HPP
#define BOID_HPP

class Boid {
    private:
        double x; //X position
        double y; //Y position
        double vx; //X velocity
        double vy; //Y velocity
    public:
        Boid(double x, double y, double vx, double vy); //Constructor
        ~Boid(); //Destructor
        double getX(); //Get X position
        double getY(); //Get Y position
        double getVx(); //Get X velocity
        double getVy(); //Get Y velocity
        bool inBounds(); //Check if boid is in bounds
        void setX(double x); //Set X position
        void setY(double y); //Set Y position
        void setVx(double vx); //Set X velocity
        void setVy(double vy); //Set Y velocity
        void updatePosition(double dt); //Update position
        void periodicBoundaryConditions(); //Periodic boundary conditions
};

#endif //BOID_HPP