#include <vector>
#include "grid.h"

#define PARTICLE_RADIUS 5


struct Particle {
    CellColour colour;
    Vi2D position;
};

struct Particles {
    std::vector<Particle> particles;
};

void printParticles(const Particles* particles);
bool particleInBounds(Vi2D pos);
void update(const Particles* original, Particles* target, const float colour_attraction[NUM_COLOURS][NUM_COLOURS] );
Vf2D getForceBetweenParticles(const Particle& particle_a, const Particle& particle_b, const float colour_attraction[NUM_COLOURS][NUM_COLOURS], float repulsion_distance, float max_distance);