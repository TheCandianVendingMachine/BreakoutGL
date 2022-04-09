// particle.hpp
// defines a particle in space. contains transform matrix, and lifespan
#pragma once
#include <glm/vec2.hpp>
#include "time.hpp"
#include "particleAccelerationCurveType.hpp"

struct particle
	{
		glm::vec2 spawnPosition = { 0, 0 };
		glm::vec2 initialVelocity = { 0, 0 };
		fe::time spawnTime = fe::seconds(0);
		fe::time killTime = fe::seconds(0);
		particleAccelerationCurveType accelerationCurve = particleAccelerationCurveType::NONE;
	};
