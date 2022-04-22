// particleRenderer.hpp
// Handles the rendering of all particles
#pragma once
#include "graphics/shader.hpp"
#include "graphics/vertexArray.hpp"
#include "particle.hpp"
#include "particleAccelerationCurveType.hpp"
#include "clock.hpp"
#include "threadJob.hpp"
#include "graphics/texture.hpp"
#include <array>
#include <plf_colony.h>
#include <glm/mat4x4.hpp>

class camera;
class particleRenderer
    {
        private:
            struct particleSSBO
                {
                    glm::vec2 spawn = { 0.f, 0.f };
                    glm::vec2 initialVelocity = { 0.f, 0.f };
                    unsigned int runtime = 0;
                    particleAccelerationCurveType curve = particleAccelerationCurveType::NONE;
                    float size = 1.f;
                    float rotation = 0.f;
                    unsigned int lifeSpan = 0;
                };
            static constexpr int c_particleSSBOAlignment = 8;
            static constexpr int c_particleSSBOOffset = c_particleSSBOAlignment + sizeof(particleSSBO) - sizeof(particleSSBO) % c_particleSSBOAlignment;

            unsigned int m_particleSSBO = 0;

            plf::colony<particle> m_particles;
            fe::clock m_internalClock;
            fe::clock *m_gameClock = nullptr;
            fe::time m_nextTimeForRemoval = fe::seconds(0);

            vertexArray m_particleVAO;

            shader m_particleShader;
            texture m_particleTextureMap;

            std::array<fe::threadFunction, 16> m_particleRenderThreads;

            fe::time getCurrentTime() const;
            bool renderParticlesToBuffer(plf::colony<particle>::iterator itBegin, plf::colony<particle>::iterator itEnd, int offset, char *&cBuffer) const;

        public:
            static constexpr int c_maxParticles = 128'000'000 / sizeof(particleSSBO);

            particleRenderer();
            void setClock(fe::clock &clock);

            void handleDestruction();
            void render(const camera &camera, unsigned int texture);

            void addParticle(glm::vec2 position, glm::vec2 initialVelocity, particleAccelerationCurveType type, fe::time lifespan, float size);
            unsigned int particleCount() const;
    };

