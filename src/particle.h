#include <vector>
#include "grid.h"

#define PARTICLE_RADIUS 5


struct Particle {
    CellColour colour;
    Vf2D position;
};

struct Particles {
    std::vector<Particle> particles;
};

float mod(float a, float b);
void printParticles(const Particles* particles);
void update(
    const Particles* original,
    Particles* target,
    const float colour_attraction[NUM_COLOURS][NUM_COLOURS],
    float dt,
    float neighbour_range, float repulsion_range
);
bool particleInBounds(Vf2D pos);
Vf2D getForceBetweenParticles(const Particle& particle_a, const Particle& particle_b, const float colour_attraction[NUM_COLOURS][NUM_COLOURS], float repulsion_distance, float max_distance);
Vf2D getShadowPoint(Vf2D a, Vf2D b);
void SaveState(Particles* p, std::string name);
void LoadState(Particles* p, std::string name);
