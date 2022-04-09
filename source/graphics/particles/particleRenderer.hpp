// particleRenderer.hpp
// Handles the rendering of all particles
#pragma once
#include "graphics/shader.hpp"
#include "graphics/vertexArray.hpp"
#include "particle.hpp"
#include "particleAccelerationCurveType.hpp"
#include "clock.hpp"
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
					float runtime = 0.f;
					particleAccelerationCurveType curve = particleAccelerationCurveType::NONE;
				};

			unsigned int m_particleSSBO = 0;

			plf::colony<particle> m_particles;
			fe::clock m_internalClock;
			fe::clock *m_gameClock = nullptr;
			fe::time m_nextTimeForRemoval = fe::seconds(0);

			vertexArray m_particleVAO;

			shader m_particleShader;

			fe::time getCurrentTime() const;

		public:
			static constexpr int c_maxParticles = 128'000'000 / sizeof(particleSSBO);

			particleRenderer();
			void setClock(fe::clock &clock);

			void handleDestruction();

			void render(const camera &camera, unsigned int texture);

			void addParticle(glm::vec2 position, glm::vec2 initialVelocity, particleAccelerationCurveType type, fe::time lifespan);
			unsigned int particleCount() const;
	};
