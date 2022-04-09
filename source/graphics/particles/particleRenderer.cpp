#include "particleRenderer.hpp"
#include "graphics/camera.hpp"
#include <glad/glad.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include "threadPool.hpp"

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

fe::time particleRenderer::getCurrentTime() const
	{
		if (m_gameClock)
			{
				return m_gameClock->getTime();
			}
		return m_internalClock.getTime();
	}

bool particleRenderer::renderParticlesToBuffer(plf::colony<particle>::iterator itBegin, plf::colony<particle>::iterator itEnd, int offset, char*& cBuffer) const
	{
		fe::time now = getCurrentTime();
		for (auto it = itBegin; it < itEnd; ++it)
			{
				particleSSBO ssbo;
				ssbo.runtime = now.thisTime - it->spawnTime.thisTime;
				ssbo.curve = it->accelerationCurve;
				ssbo.spawn = it->spawnPosition;
				ssbo.initialVelocity = it->initialVelocity;
				ssbo.size = it->size;
				ssbo.rotation = glm::radians(20.f);

				// manually unrolling loop gains us some frametime
				cBuffer[offset + 0] = reinterpret_cast<char*>(&ssbo)[0];
				cBuffer[offset + 1] = reinterpret_cast<char*>(&ssbo)[1];
				cBuffer[offset + 2] = reinterpret_cast<char*>(&ssbo)[2];
				cBuffer[offset + 3] = reinterpret_cast<char*>(&ssbo)[3];
				cBuffer[offset + 4] = reinterpret_cast<char*>(&ssbo)[4];
				cBuffer[offset + 5] = reinterpret_cast<char*>(&ssbo)[5];
				cBuffer[offset + 6] = reinterpret_cast<char*>(&ssbo)[6];
				cBuffer[offset + 7] = reinterpret_cast<char*>(&ssbo)[7];
				cBuffer[offset + 8] = reinterpret_cast<char*>(&ssbo)[8];
				cBuffer[offset + 9] = reinterpret_cast<char*>(&ssbo)[9];
				cBuffer[offset + 10] = reinterpret_cast<char*>(&ssbo)[10];
				cBuffer[offset + 11] = reinterpret_cast<char*>(&ssbo)[11];
				cBuffer[offset + 12] = reinterpret_cast<char*>(&ssbo)[12];
				cBuffer[offset + 13] = reinterpret_cast<char*>(&ssbo)[13];
				cBuffer[offset + 14] = reinterpret_cast<char*>(&ssbo)[14];
				cBuffer[offset + 15] = reinterpret_cast<char*>(&ssbo)[15];
				cBuffer[offset + 16] = reinterpret_cast<char*>(&ssbo)[16];
				cBuffer[offset + 17] = reinterpret_cast<char*>(&ssbo)[17];
				cBuffer[offset + 18] = reinterpret_cast<char*>(&ssbo)[18];
				cBuffer[offset + 19] = reinterpret_cast<char*>(&ssbo)[19];
				cBuffer[offset + 20] = reinterpret_cast<char*>(&ssbo)[20];
				cBuffer[offset + 21] = reinterpret_cast<char*>(&ssbo)[21];
				cBuffer[offset + 22] = reinterpret_cast<char*>(&ssbo)[22];
				cBuffer[offset + 23] = reinterpret_cast<char*>(&ssbo)[23];
				cBuffer[offset + 24] = reinterpret_cast<char*>(&ssbo)[24];
				cBuffer[offset + 25] = reinterpret_cast<char*>(&ssbo)[25];
				cBuffer[offset + 26] = reinterpret_cast<char*>(&ssbo)[26];
				cBuffer[offset + 27] = reinterpret_cast<char*>(&ssbo)[27];
				cBuffer[offset + 28] = reinterpret_cast<char*>(&ssbo)[28];
				cBuffer[offset + 29] = reinterpret_cast<char*>(&ssbo)[29];
				cBuffer[offset + 30] = reinterpret_cast<char*>(&ssbo)[30];
				cBuffer[offset + 31] = reinterpret_cast<char*>(&ssbo)[31];

				offset += sizeof(particleSSBO);
			}
		return true;
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
		m_particleVAO.use(vertex::attributes::POSITION | vertex::attributes::TEXTURE);

		m_particleTextureMap.loadFromFile("foo.png", false);
	}

void particleRenderer::setClock(fe::clock &clock)
	{
		m_gameClock = &clock;
	}

void particleRenderer::handleDestruction()
	{
		fe::time currentTime = getCurrentTime();
		if (currentTime >= m_nextTimeForRemoval)
			{
				m_nextTimeForRemoval = fe::time::max;
				for (auto it = m_particles.begin(); it != m_particles.end();)
					{
						if (currentTime >= it->killTime)
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
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particleSSBO);
				void *buffer = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
				char *cBuffer = reinterpret_cast<char*>(buffer);
				
				if (m_particles.size() > 1'000'000) 
					{
						int i = 0;
						int particleFraction = m_particles.size() / m_particleRenderThreads.size();
						for (auto &particleThread : m_particleRenderThreads)
							{
								plf::colony<particle>::iterator itBegin = m_particles.begin();
								plf::colony<particle>::iterator itEnd = m_particles.begin(); std::advance(itEnd, 1 * particleFraction);

								particleThread = fe::threadFunction(std::bind(&particleRenderer::renderParticlesToBuffer, this, itBegin, itEnd, sizeof(particleSSBO) * i * particleFraction, cBuffer));
								globals::g_threadPool->addJob(particleThread);
							}
						
						for (auto& particleThread : m_particleRenderThreads) 
							{
								globals::g_threadPool->waitFor(particleThread);
							}
					}
				else
					{
						renderParticlesToBuffer(m_particles.begin(), m_particles.end(), 0, cBuffer);
					}
				
				glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

				m_particleShader.use();

				m_particleShader.setMat4("view", camera.view());
				m_particleShader.setMat4("projection", camera.projection());
				m_particleShader.setStorageBuffer("ParticleBuffer", m_particleSSBO);

				m_particleTextureMap.bind(GL_TEXTURE0);

				glBindVertexArray(m_particleVAO.vao);
				glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, m_particles.size());
			}
	}

void particleRenderer::addParticle(glm::vec2 position, glm::vec2 initialVelocity, particleAccelerationCurveType type, fe::time lifespan, float size)
	{
		if (m_particles.size() >= c_maxParticles) { return; }

		particle p;
		p.spawnTime = getCurrentTime();
		p.killTime = p.spawnTime + lifespan;
		p.spawnPosition = position;
		p.initialVelocity = initialVelocity;
		p.accelerationCurve = type;
		p.size = size;

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
