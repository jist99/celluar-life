#include "particle.h"
#include <iostream>
#include <cmath>

void update(const Particles* original, Particles* target, const float colour_attraction[NUM_COLOURS][NUM_COLOURS]) {
    const float neighbour_range = 800;
    const float repulsion_range = 16;

    std::vector<int> to_erase = {}; 

    //get force acting on each particle
    for (int i = 0; i < original->particles.size(); i++)
    {
        Particle p = original->particles[i];

        Vf2D force = {0,0};
        //get force of every other particle on this one TODO: only consider those within neighbour_range (optimise by keeping vector sorted?)
        for (int j = 0; j < original->particles.size(); j++)
        {
            if (j == i) continue;
            Particle other_p = original->particles[j];
            force += getForceBetweenParticles(p, other_p, colour_attraction, repulsion_range, neighbour_range);
        }

        //normalise force (TODO: done this way for parity for now may want to just apply force directly to particles)
        //force = force.norm();

        std::cout << force.str() << std::endl;
        //todo: fix how force normalisation works

        //round force to number of pixels to move
        force.x = std::round(force.x);
        force.y = std::round(force.y);

        //update position of particle in next gen with force
        Vi2D new_pos = original->particles.at(i).position + (Vi2D)force;

        //if position is out of bounds, remove it (TODO: is this what we want?)
        if(!particleInBounds(new_pos))
        {
            to_erase.push_back(i);
        }
        else
            target->particles.at(i).position = new_pos;
    }
    for (int i_erase : to_erase)
        target->particles.erase(target->particles.begin() + i_erase);
}

Vf2D getForceBetweenParticles(const Particle& particle_a, const Particle& particle_b, const float colour_attraction[NUM_COLOURS][NUM_COLOURS], float repulsion_distance, float max_distance)
{
    //get coefficient between colours (b acting on a)
    float coeff = colour_attraction[(particle_a.colour) - 1][(particle_b.colour) - 1] * 16;

    //calculate distance between cells
    float distance = particle_a.position.distance(particle_b.position);
    
    float magnitude = 0;

    //calculate magnitude of force between cells
    if(distance < repulsion_distance)
    {
        //linear force increase from -1 at d=0 to 0 at repulsion_distance
        magnitude = (distance/repulsion_distance) - 1;
    }
    else if (distance < ((max_distance-repulsion_distance)/2))
    {
        //linear force increase up to max value of coeff
        magnitude = ((2*coeff) / (max_distance - 3*repulsion_distance)) * (distance - repulsion_distance);
    }
    else if (distance < max_distance)
    {
        //linear force decrease to 0 at max_distance
        magnitude = -((2*coeff) / (max_distance - 3*repulsion_distance)) * (distance - max_distance);
    }

    //return x and y component of force
    return Vf2D{magnitude*(float(particle_b.position.x-particle_a.position.x)/distance), magnitude*(float(particle_b.position.y-particle_a.position.y)/distance)};
}

bool particleInBounds(Vi2D pos)
{
    return (pos.x >= 0 && pos.x < GetScreenWidth() && pos.y >= 0 && pos.y < GetScreenHeight());
}

void printParticles(const Particles* particles)
{
    for (Particle p : particles->particles)
    {
        std::cout << "x:" << p.position.x << " " << "y:" << p.position.y << " " << "colour:" << p.colour << std::endl;
    }
}
