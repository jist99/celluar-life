#include "particle.h"
#include "grid.h" // This is just to get GRID_WIDTH and HEIGHT, maybe move it?
#include <iostream>
#include <cmath>
#include <algorithm>

void update(
    const Particles* original, Particles* target,
    const float colour_attraction[NUM_COLOURS][NUM_COLOURS],
    float dt
) {
    const float neighbour_range = 16;
    const float repulsion_range = 2;

    // std::vector<int> to_erase = {}; 

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
            force += getForceBetweenParticles(
                p, other_p, colour_attraction, repulsion_range, neighbour_range
            );
        }

        //update position of particle in next gen with force
        Vf2D new_pos = original->particles.at(i).position + (force * dt);

        // //mod position to ensure in bounds and wrap around if not
        // new_pos.x = mod(new_pos.x, GetScreenWidth());
        // new_pos.y = mod(new_pos.y, GetScreenHeight());
        // target->particles.at(i).position = new_pos;

        //if position is out of bounds, wrap round
        if(!particleInBounds(new_pos))
        {
            // std::cout << "wrapped" << std::endl;
            // std::cout << "force" << force.x << "," << force.y << std::endl;
            // std::cout << "pos" << original->particles.at(i).position.x << "," << original->particles.at(i).position.y << std::endl;
            target->particles.at(i).position = getWrappedPoint(original->particles.at(i).position, new_pos, force);
            // std::cout << "newpos" << target->particles.at(i).position.x << "," << target->particles.at(i).position.y << std::endl;
        }
        else
            target->particles.at(i).position = new_pos;
    }
    // for (int i_erase : to_erase)
    //     target->particles.erase(target->particles.begin() + i_erase);
}

Vf2D getForceBetweenParticles(
    const Particle& particle_a,
    const Particle& particle_b,
    const float colour_attraction[NUM_COLOURS][NUM_COLOURS],
    float repulsion_distance, float max_distance
){
    //get coefficient between colours (b acting on a)
    float coeff = colour_attraction[(particle_a.colour) - 1][(particle_b.colour) - 1];

    //calculate distance between cells
    float distance = particle_a.position.distance(particle_b.position);

    //calculate a "b'" (shadow particle that represents position relative to a considering wrapping around the screen)

    Vf2D b_prime = getShadowPoint(particle_a.position, particle_b.position);

    float wrap_distance = particle_a.position.distance(b_prime);

    Vf2D b_pos = particle_b.position;

    //if the particles are closer when considering wrapping, consider that instead
    if(wrap_distance < distance)
    {
        distance = wrap_distance;
        b_pos = b_prime;
    }
    
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
    return Vf2D{magnitude*(float(b_pos.x-particle_a.position.x)/distance), magnitude*(float(b_pos.y-particle_a.position.y)/distance)};
}

bool particleInBounds(Vf2D pos)
{
    return (pos.x >= 0 && pos.x < GRID_WIDTH && pos.y >= 0 && pos.y < GRID_HEIGHT);
}

void printParticles(const Particles* particles)
{
    for (Particle p : particles->particles)
    {
        std::cout << "x:" << p.position.x << " " << "y:" << p.position.y << " " << "colour:" << p.colour << std::endl;
    }
}

//custom mod function as fmod handles negatives strangely
float mod(float a, float b)
{
    float mod = std::fmod(a,b);
    if (mod < 0)
        mod += b;
    return mod;
}

//function to get a "shadow" version of b off of the screen that repsresents a point equivilent to wrapping from b across the screen to a
Vf2D getShadowPoint(Vf2D a, Vf2D b)
{
    float width = GRID_WIDTH;
    float height = GRID_HEIGHT;

    //if vertical
    if (b.x == a.x)
    {
        if(height < 2*b.y)
            return Vf2D{b.x, b.y-height};
        else
            return Vf2D{b.x, b.y+height};
    }
    //if horizontal
    if (b.y == a.y)
    {
        if(width < 2*b.x)
            return Vf2D{b.x-width, b.y};
        else
            return Vf2D{b.x+width, b.y};
    }

    float grad = (b.y - a.y)/(b.x - a.x);
    float intercept = a.y-(grad*a.x);

    float right_dist_b = width - b.x;
    float bottom_dist_b = height - b.y;

    float right_dist_a = width - a.x;
    float bottom_dist_a = height - a.y;

    //intersect points with closest edge
    Vf2D b_intersect = {};
    Vf2D a_intersect = {};

    //get intersect of b and its closest edge (TODO: div zero checking)
    if(b.x <= b.y && b.x <= right_dist_b && b.x <= bottom_dist_b)
    {
        //intersect with x = 0
        b_intersect.y = intercept;
    }
    else if (b.y <= b.x && b.y <= right_dist_b && b.y <= bottom_dist_b)
    {
        //intersect with y = 0
        b_intersect.x = -intercept/grad;
    }
    else if (right_dist_b <= b.x && right_dist_b <= b.y && right_dist_b <= bottom_dist_b)
    {
        //intersect with x = width
        b_intersect.x = width;
        b_intersect.y = grad*width + intercept;
    }
    else if (bottom_dist_b <= b.x && bottom_dist_b <= b.y && bottom_dist_b <= right_dist_b)
    {
        //intersect with y = height
        b_intersect.y = height;
        b_intersect.x = (height - intercept)/grad;
    }
    else
    {
        //shouldn't happen - abort and return original
        return b;
    }

    //get intersect of a and its closest edge (TODO: div zero checking)
    if(a.x <= a.y && a.x <= right_dist_a && a.x <= bottom_dist_a)
    {
        //intersect with x = 0
        a_intersect.y = intercept;
    }
    else if (a.y <= a.x && a.y <= right_dist_a && a.y <= bottom_dist_a)
    {
        //intersect with y = 0
        a_intersect.x = -intercept/grad;
    }
    else if (right_dist_a <= a.x && right_dist_a <= a.y && right_dist_a <= bottom_dist_a)
    {
        //intersect with x = width
        a_intersect.x = width;
        a_intersect.y = grad*width + intercept;
    }
    else if (bottom_dist_a <= a.x && bottom_dist_a <= a.y && bottom_dist_a <= right_dist_a)
    {
        //intersect with y = height
        a_intersect.y = height;
        a_intersect.x = (height - intercept)/grad;
    }
    else
    {
        //shouldn't happen - abort and return original
        return b;
    }

    return a_intersect + (b - b_intersect);
}

//given that a point a has been moved to a point b off screen, calculate where it should wrap to
Vf2D getWrappedPoint(Vf2D a, Vf2D b, Vf2D force)
{
    float width = GRID_WIDTH;
    float height = GRID_HEIGHT;

    //if vertical
    if (b.x == a.x)
    {
        return Vf2D{b.x, mod(b.y, height)};
    }
    //if horizontal
    if (b.y == a.y)
    {
        return Vf2D{mod(b.x, width), b.y};
    }

    float grad = (b.y - a.y)/(b.x - a.x);
    float intercept = a.y-(grad*a.x);

    //points where the particle exits and enters the screen
    Vf2D intersect = {};
    Vf2D exit_point = {};
    Vf2D entry_point = {};

    //calculate intersection points with all four edges, keeping the two that are within our boundries

    //intersect with x = width
    intersect = {width,grad*width + intercept};
    if(particleInBounds(intersect)) {
        if(intersect.x <= std::max(a.x, b.x) && intersect.x >= std::min(a.x, b.x) && intersect.y <= std::max(a.y, b.y) && intersect.y >= std::min(a.y, b.y))
            exit_point = intersect;
        else
            entry_point = intersect;
    }
    //intersect with x = 0
    intersect = {0, intercept};
    if(particleInBounds(intersect)) {
        if(intersect.x <= std::max(a.x, b.x) && intersect.x >= std::min(a.x, b.x) && intersect.y <= std::max(a.y, b.y) && intersect.y >= std::min(a.y, b.y))
            exit_point = intersect;
        else
            entry_point = intersect;
    }
    //intersect with y = height
    intersect = {(height - intercept)/grad,height};
    if(particleInBounds(intersect)) {
        if(intersect.x <= std::max(a.x, b.x) && intersect.x >= std::min(a.x, b.x) && intersect.y <= std::max(a.y, b.y) && intersect.y >= std::min(a.y, b.y))
            exit_point = intersect;
        else
            entry_point = intersect;
    }
    //intersect with y = 0
    intersect = {-intercept/grad, 0};
    if(particleInBounds(intersect)) {
        if(intersect.x <= std::max(a.x, b.x) && intersect.x >= std::min(a.x, b.x) && intersect.y <= std::max(a.y, b.y) && intersect.y >= std::min(a.y, b.y))
            exit_point = intersect;
        else
            entry_point = intersect;
    }

    return entry_point + (b-exit_point);
}
