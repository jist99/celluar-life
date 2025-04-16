#include "particle.h"
#include "grid.h" // This is just to get GRID_WIDTH and HEIGHT, maybe move it?
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <filesystem>

void update(
    const Particles* original, Particles* target,
    const float colour_attraction[NUM_COLOURS][NUM_COLOURS],
    float dt, float neighbour_range, float repulsion_range
) {

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

        //if position is out of bounds, wrap round
        if(!particleInBounds(new_pos))
        {
            target->particles.at(i).position = {mod(new_pos.x, GRID_WIDTH),mod(new_pos.y, GRID_HEIGHT)};
        }
        else
            target->particles.at(i).position = new_pos;
    }
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
        magnitude = ((2*coeff) / (max_distance - repulsion_distance)) * (distance - repulsion_distance);
    }
    else if (distance < max_distance)
    {
        //linear force decrease to 0 at max_distance
        magnitude = ((2*coeff) / (repulsion_distance - max_distance)) * (distance - max_distance);
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
    //calculate shadow versions of b on 4 adjacent plains extending offscreen in each direction
    Vf2D b_left = {b.x-GRID_WIDTH, b.y};
    Vf2D b_right = {b.x+GRID_WIDTH, b.y};
    Vf2D b_top = {b.x, b.y-GRID_HEIGHT};
    Vf2D b_bottom = {b.x, b.y+GRID_HEIGHT};

    //find closest point to a
    float left_dist = a.distance(b_left);
    float right_dist = a.distance(b_right);
    float top_dist = a.distance(b_top);
    float bottom_dist = a.distance(b_bottom);

    float min_distance = std::min({left_dist, right_dist, top_dist, bottom_dist});

    if(min_distance == left_dist)
        return b_left;
    else if(min_distance == right_dist)
        return b_right;
    else if(min_distance == top_dist)
        return b_top;
    else
        return b_bottom;

}

void SaveParticles(Particles* p, std::string name)
{
    std::string path = "saved_states/" + name;
    //if not ending in .particle, add it
    if(!EndsWith(path,".particle"))  path += ".particle";
    std::ofstream file_stream(path, std::ios::out | std::ios::binary);
    if (!file_stream)
    {
        std::cerr << "Failed to open file" << std::endl;
        return;
    }
    //write particle data, as well as size to allow reconstruction
    size_t size = p->particles.size();
    file_stream.write(reinterpret_cast<char*>(&size), sizeof(size));
    file_stream.write(reinterpret_cast<char*>(p->particles.data()), sizeof(Particle) * size);
    file_stream.close();
}

void LoadParticles(Particles* current_p, Particles* next_p, std::string name)
{
    //if not ending in .particle or .grid, check for instances of either, prioritising .particle
    if(!EndsWith(name,".particle") && !EndsWith(name,".grid"))
    {
        if(std::filesystem::exists("saved_states/" + name + ".particle")) name += ".particle";
        else if(std::filesystem::exists("saved_states/" + name + ".grid")) name += ".grid";
    }
    //if you are loading a .grid file, convert to particle
    if(EndsWith(name,".grid"))
    {
        ConvertCell2Particle(current_p, name);
        next_p->particles = current_p->particles;
    }
    else
    {
        std::string path = "saved_states/" + name;
        std::ifstream file_stream(path, std::ios::in | std::ios::binary);
        if (!file_stream)
        {
            std::cerr << "Failed to open file" << std::endl;
            return;
        }
        //Read particle data into existing struct, reading size first to ensure correct amount of space
        size_t size;
        file_stream.read(reinterpret_cast<char*>(&size), sizeof(size));
        current_p->particles.resize(size);
        file_stream.read(reinterpret_cast<char*>(current_p->particles.data()), sizeof(Particle) * size);
        file_stream.close();
        next_p->particles = current_p->particles;
    }
}

void ConvertCell2Particle(Particles* p, std::string name)
{
    //clear particles
    *p = {};
    //load in cells
    Grid grid = {};
    Grid* g = &grid;
    LoadGrid(g, name);
    //for each cell, convert to a particle
    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++) {
        if (g->colour[i] == CellColour::Blank) continue;
        Particle part;
        part.colour = g->colour[i];
        part.position = Vf2D(gridXY(i));
        p->particles.push_back(part);
    }
}