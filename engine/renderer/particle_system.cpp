//
// Created by Nemo li on 2023/12/14.
//

#include <render_async/gpu_resource_mapper.h>
#include "particle_system.h"
#include "helpers/time.h"
#include "helpers/debug.h"
#include "renderer/camera.h"
#include "shader.h"

RTTR_REGISTRATION {
    rttr::registration::class_<ParticleSystem>("ParticleSystem")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

float *ParticleSystem::g_particle_position_size_data = new float[MAX_PARTICLES * 4];
unsigned char *ParticleSystem::g_particle_color_data = new unsigned char[MAX_PARTICLES * 4];

const float ParticleSystem::g_vertex_buffer_data[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
};

ParticleSystem::ParticleSystem() {

}

void ParticleSystem::Update() {
    float deltaTime = Time::delta_time();
//    DEBUG_LOG_INFO("ParticleSystem update delta time: {}", deltaTime);
    CreateParticle(deltaTime);

    SimulateParticles(deltaTime);

    SortParticles();
}

void ParticleSystem::CreateParticle(float deltaTime) {
    int new_particles = (int) (deltaTime * 1000);
    if (new_particles > (int) (0.016f * 1000)) {
        new_particles = (int) (0.016f * 1000);
    }

    for (int i = 0; i < new_particles; ++i) {
        int unusedParticle = FindUnusedParticle();
//        DEBUG_LOG_INFO("unusedParticle: {}", unusedParticle);
        // This particle will live 5 seconds.
        ParticlesContainer[unusedParticle].life = 5.0f;
        ParticlesContainer[unusedParticle].pos = glm::vec3(0, 0, -5.0f);

        float spread = 1.5f;
        glm::vec3 main_dir = glm::vec3(0.0f, 10.0f, 0.0f);

        glm::vec3 random_dir = glm::vec3(
                (rand() % 2000 - 1000.0f) / 1000.0f,
                (rand() % 2000 - 1000.0f) / 1000.0f,
                (rand() % 2000 - 1000.0f) / 1000.0f
        );

        ParticlesContainer[unusedParticle].speed = main_dir + random_dir * spread;

        // Very bad way to generate a random color
        ParticlesContainer[unusedParticle].r = rand() % 256;
        ParticlesContainer[unusedParticle].g = rand() % 256;
        ParticlesContainer[unusedParticle].b = rand() % 256;
        ParticlesContainer[unusedParticle].a = (rand() % 256) / 3;

        ParticlesContainer[unusedParticle].size = (rand() % 1000) / 2000.0f + 0.1f;
    }
}

int ParticleSystem::FindUnusedParticle() {
    for (int i = last_used_particle_; i < MAX_PARTICLES; ++i) {
        if (ParticlesContainer[i].life < 0) {
            last_used_particle_ = i;
            return i;
        }
    }
    for (int j = 0; j < last_used_particle_; ++j) {
        if (ParticlesContainer[j].life < 0) {
            last_used_particle_ = j;
            return j;
        }
    }

    // All particles are taken, override the first one
    return 0;
}

void ParticleSystem::SortParticles() {
    std::sort(&ParticlesContainer[0], &ParticlesContainer[MAX_PARTICLES]);
}

void ParticleSystem::SimulateParticles(float deltaTime) {
    Camera *camera = Camera::current_camera();
    if (camera == nullptr) {
        return;
    }
    glm::mat4 &viewMat = camera->view_mat4();
    glm::vec3 CameraPosition(glm::inverse(viewMat)[3]);

    ParticlesCount = 0;

    // Simulate all particles
    for (int i = 0; i < MAX_PARTICLES; i++) {

        Particle &p = ParticlesContainer[i]; // shortcut

        if (p.life > 0.0f) {
            // Decrease life
            p.life -= deltaTime;
//            DEBUG_LOG_INFO("p life {}", p.life);

            if (p.life > 0.0f) {
                // Simulate simple physics : gravity only, no collisions
                p.speed += glm::vec3(0.0f, -9.81f, 0.0f) * (float) deltaTime * 0.5f;
                p.pos += p.speed * (float) deltaTime;
                p.camera_distance = glm::length(p.pos - CameraPosition);
                //ParticlesContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

                // Fill the GPU buffer
                g_particle_position_size_data[4 * ParticlesCount + 0] = p.pos.x;
                g_particle_position_size_data[4 * ParticlesCount + 1] = p.pos.y;
                g_particle_position_size_data[4 * ParticlesCount + 2] = p.pos.z;

                g_particle_position_size_data[4 * ParticlesCount + 3] = p.size;

                g_particle_color_data[4 * ParticlesCount + 0] = p.r;
                g_particle_color_data[4 * ParticlesCount + 1] = p.g;
                g_particle_color_data[4 * ParticlesCount + 2] = p.b;
                g_particle_color_data[4 * ParticlesCount + 3] = p.a;
            } else {
                // Particles that just died will be put at the end of the buffer in SortParticles();
                p.camera_distance = -1.0f;
            }
            ParticlesCount++;
        }
    }
}

void ParticleSystem::SetMaterial(Material *material) {
    material_ = material;
}

void ParticleSystem::Render() {
    Shader *shader = material_->shader();
    unsigned int shaderProgramHandle = shader->shader_program_handle();
    GLuint shaderProgram = GPUResourceMapper::GetShaderProgram(shaderProgramHandle);
//    DEBUG_LOG_INFO("shaderProgram {}", shaderProgram);
//    GLint attribute_pos_vertex = glGetAttribLocation(shaderProgram, "squareVertices");
//    __CHECK_GL_ERROR__
//    GLint attribute_pos_center = glGetAttribLocation(shaderProgram, "xyzs");
//    __CHECK_GL_ERROR__
//    GLint attribute_pos_color = glGetAttribLocation(shaderProgram, "color");
//    __CHECK_GL_ERROR__

    if (vao_ == 0) {
        glGenVertexArrays(1, &vao_);
        __CHECK_GL_ERROR__
        glBindVertexArray(vao_);
        __CHECK_GL_ERROR__

        glGenBuffers(1, &billboard_vertex_buffer);
        __CHECK_GL_ERROR__
        glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
        __CHECK_GL_ERROR__
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
        __CHECK_GL_ERROR__

        glGenBuffers(1, &particles_position_buffer);
        __CHECK_GL_ERROR__
        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        __CHECK_GL_ERROR__
        // Initialize with empty (NULL) buffer : it will be updated later, each frame.
        glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), nullptr, GL_STREAM_DRAW);
        __CHECK_GL_ERROR__

        glGenBuffers(1, &particles_color_buffer);
        __CHECK_GL_ERROR__
        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
        __CHECK_GL_ERROR__

        // Initialize with empty (NULL) buffer : it will be updated later, each frame.
        glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), nullptr, GL_STREAM_DRAW);
        __CHECK_GL_ERROR__

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        __CHECK_GL_ERROR__
        glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
        __CHECK_GL_ERROR__
        glVertexAttribPointer(
                0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void *) 0            // array buffer offset
        );
        __CHECK_GL_ERROR__

        // 2nd attribute buffer : positions of particles' centers
        glEnableVertexAttribArray(1);
        __CHECK_GL_ERROR__
        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        __CHECK_GL_ERROR__
        glVertexAttribPointer(
                1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                4,                                // size : x + y + z + size => 4
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void *) 0                          // array buffer offset
        );
        __CHECK_GL_ERROR__

        // 3rd attribute buffer : particles' colors
        glEnableVertexAttribArray(2);
        __CHECK_GL_ERROR__
        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
        __CHECK_GL_ERROR__
        glVertexAttribPointer(
                2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                4,                                // size : r + g + b + a => 4
                GL_UNSIGNED_BYTE,                 // type
                GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
                0,                                // stride
                (void *) 0                          // array buffer offset
        );
        __CHECK_GL_ERROR__

        glBindVertexArray(0);
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        __CHECK_GL_ERROR__
        glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), nullptr,
                     GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
        __CHECK_GL_ERROR__
        glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, g_particle_position_size_data);
        __CHECK_GL_ERROR__

        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
        __CHECK_GL_ERROR__
        glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), nullptr,
                     GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
        __CHECK_GL_ERROR__
        glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, g_particle_color_data);
        __CHECK_GL_ERROR__
    }

    glBindVertexArray(vao_);
    __CHECK_GL_ERROR__

    glEnable(GL_BLEND);
    __CHECK_GL_ERROR__
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    __CHECK_GL_ERROR__

    // Use our shader
    shader->Use();

    // These functions are specific to glDrawArrays*Instanced*.
    // The first parameter is the attribute buffer we're talking about.
    // The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
    // http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
    glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
    __CHECK_GL_ERROR__
    glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
    __CHECK_GL_ERROR__
    glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1
    __CHECK_GL_ERROR__

    // Draw the particules !
    // This draws many times a small triangle_strip (which looks like a quad).
    // This is equivalent to :
    // for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4),
    // but faster.
    if (ParticlesCount != 0) {
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);
        __CHECK_GL_ERROR__
    }

    glDisableVertexAttribArray(0);
    __CHECK_GL_ERROR__
    glDisableVertexAttribArray(1);
    __CHECK_GL_ERROR__
    glDisableVertexAttribArray(2);
    __CHECK_GL_ERROR__
}
