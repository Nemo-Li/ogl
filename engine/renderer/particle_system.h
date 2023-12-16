//
// Created by Nemo li on 2023/12/14.
//

#ifndef OGL_PARTICLE_SYSTEM_H
#define OGL_PARTICLE_SYSTEM_H

#include "component/component.h"
#include "glm/glm.hpp"
#include "material.h"

#define MAX_PARTICLES 10000

class ParticleSystem : public Component {
public:
    static float *g_particle_position_size_data;
    static unsigned char *g_particle_color_data;

    struct Particle {
        glm::vec3 pos, speed;
        //color
        unsigned char r, g, b, a;
        float size, angle, weight;
        float life;
        // *Squared* distance to the camera. if dead : -1.0f
        float camera_distance;

        bool operator<(const Particle &that) const {
            // Sort in reverse order : far particles drawn first.
            return this->camera_distance > that.camera_distance;
        }
    };

    Particle ParticlesContainer[MAX_PARTICLES];

    ParticleSystem();

    void Update() override;

    //设置Material
    void SetMaterial(Material *material);

    //渲染
    void Render();

    Material *material() { return material_; }

private:
    Material *material_;

    int last_used_particle_;

    int ParticlesCount = 0;

    void CreateParticle(float deltaTime);

    void SortParticles();

    void SimulateParticles(float deltaTime);

    int FindUnusedParticle();

    //vbo containing the 4 vertices of the particles.
    unsigned int billboard_vertex_buffer_handle_ = 0;
    //The VBO containing the positions and sizes of the particles
    unsigned int particles_position_buffer_handle_ = 0;
    // The VBO containing the colors of the particles
    unsigned int particles_color_buffer_handle_ = 0;

    unsigned int vao_ = 0;

    unsigned int billboard_vertex_buffer;

    unsigned int particles_position_buffer;

    unsigned int particles_color_buffer;

    static const float g_vertex_buffer_data[];

RTTR_ENABLE();
};


#endif //OGL_PARTICLE_SYSTEM_H
