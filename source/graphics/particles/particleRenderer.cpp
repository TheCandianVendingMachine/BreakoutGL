#include "particleRenderer.hpp"
#include "graphics/camera.hpp"
#include <glad/glad.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

fe::time particleRenderer::getCurrentTime() const
	{
		if (m_gameClock)
			{
				return m_gameClock->getTime();
			}
		return m_internalClock.getTime();
	}

particleRenderer::particleRenderer() :
	m_particleShader("shaders/particles.vs", "shaders/particles.fs")
	{
		glGenBuffers(1, &m_particleSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particleSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, c_maxParticles * sizeof(particleSSBO), 0, GL_STREAM_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		spdlog::info("buffer {}", m_particleSSBO);

		vertex v0{
            .position = { 0.f, 0.f, 0.f },
            .textureCoordinate = { 0.f, 1.f },
            .colour = { 1.f, 1.f, 1.f }
        };
        vertex v1{
            .position = { 1.f, 0.f, 0.f },
            .textureCoordinate = { 1.f, 1.f },
            .colour = { 1.f, 1.f, 1.f }
        };
        vertex v2{
            .position = { 1.f, 1.f, 0.f },
            .textureCoordinate = { 1.f, 0.f },
            .colour = { 1.f, 1.f, 1.f }
        };
        vertex v3{
            .position = { 0.f, 1.f, 0.f },
            .textureCoordinate = { 0.f, 0.f },
            .colour = { 1.f, 1.f, 1.f }
        };

		m_particleVAO.bindVertices({ v0, v1, v2, v3 });
		m_particleVAO.bindIndices({ 0, 1, 2, 2, 3, 0 });
		m_particleVAO.use(vertex::attributes::POSITION | vertex::attributes::COLOUR);
	}

void particleRenderer::setClock(fe::clock &clock)
	{
		m_gameClock = &clock;
	}

void particleRenderer::handleDestruction()
	{
		if (getCurrentTime() >= m_nextTimeForRemoval)
			{
				m_nextTimeForRemoval = fe::time::max;
				for (auto it = m_particles.begin(); it != m_particles.end();)
					{
						if (getCurrentTime() >= it->killTime)
							{
								it = m_particles.erase(it);
							}
						else
							{
								if (it->killTime < m_nextTimeForRemoval)
									{
										m_nextTimeForRemoval = it->killTime;
									}
								++it;
							}
					}
			}
	}

void particleRenderer::render(const camera &camera, unsigned int texture)
	{
		if (!m_particles.empty())
			{
				std::vector<particleSSBO> ssboParticles(m_particles.size());
				int i = 0;
				for (auto &particle : m_particles)
					{
						ssboParticles[i].curve = particle.accelerationCurve;
						ssboParticles[i].spawn = particle.spawnPosition;
						ssboParticles[i].initialVelocity = particle.initialVelocity;
						ssboParticles[i].runtime = (getCurrentTime() - particle.spawnTime).asSeconds();
						i++;
					}
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particleSSBO);
				void* buffer = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
				std::memcpy(buffer, ssboParticles.data(), ssboParticles.size() * sizeof(particleSSBO));
				glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

				m_particleShader.use();

				m_particleShader.setMat4("view", camera.view());
				m_particleShader.setMat4("projection", camera.projection());
				m_particleShader.setStorageBuffer("ParticleBuffer", m_particleSSBO);

				glBindVertexArray(m_particleVAO.vao);
				glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, m_particles.size());
			}
	}

void particleRenderer::addParticle(glm::vec2 position, glm::vec2 initialVelocity, particleAccelerationCurveType type, fe::time lifespan)
	{
		if (m_particles.size() >= c_maxParticles) { return; }

		particle p;
		p.spawnTime = getCurrentTime();
		p.killTime = p.spawnTime + lifespan;
		p.spawnPosition = position;
		p.initialVelocity = initialVelocity;
		p.accelerationCurve = type;

		if (p.killTime < m_nextTimeForRemoval)
			{
				m_nextTimeForRemoval = p.killTime;
			}

		m_particles.emplace(std::move(p));
	}

unsigned int particleRenderer::particleCount() const
	{
		return m_particles.size();
	}
