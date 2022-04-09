#include "graphics/window.hpp"
#include "graphics/shader.hpp"
#include "graphics/camera.hpp"
#include "graphics/texture.hpp"
#include "graphics/transformable.hpp"
#include "graphics/graphicsEngine.hpp"
#include "graphics/particles/particleRenderer.hpp"

#include "threadPool.hpp"
#include "random.hpp"
#include "clock.hpp"
#include "time.hpp"
#include "str.hpp"
#include "inputHandler.hpp"

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "ecs/graphicsSystem.hpp"

#include "game/states/gameStateMachine.hpp"
#include "game/states/breakout.hpp"

int main()
    {
        spdlog::set_level(spdlog::level::debug);
        globals::commonThreadPool threadPool;
        globals::g_threadPool = &threadPool;

        fe::randomImpl c_generator{};
        c_generator.startUp();
        c_generator.seed(1337);

        window app(500, 1000, "Breakout");
        glfwSetInputMode(app.getWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

        particleRenderer particles;

        graphicsSystem graphicsSystem;
        graphicsEngine graphicsEngine(app, graphicsSystem, particles);

        globals::g_graphicsSystem = &graphicsSystem;
        globals::g_graphicsEngine = &graphicsEngine;

        inputHandler c_inputHandler(app.getWindow(), "inputs.ini");
        globals::g_inputs = &c_inputHandler;

        c_inputHandler.addDefaultInput("debug", "close", GLFW_KEY_ESCAPE);
        c_inputHandler.addDefaultInput("debug", "left", GLFW_MOUSE_BUTTON_LEFT);

        c_inputHandler.save("inputs.ini");

        constexpr fe::time simulationRate = fe::seconds(1.f / 60.f);
        constexpr fe::time maxDeltaTime = fe::seconds(1);
        fe::clock runClock;
        fe::time lastTime = runClock.getTime();
        fe::time accumulator;

        gameStateMachine game;
        game.push<breakout>();

        particles.setClock(runClock);

        constexpr int avgFPSCount = 5;
        fe::time avgFPS[avgFPSCount];
        int avgFPSIndex = 0;

        while (app.isOpen())
            {
                fe::time currentTime = runClock.getTime();
                fe::time deltaTime = currentTime - lastTime;
                if (deltaTime >= maxDeltaTime)
                    {
                        deltaTime = maxDeltaTime;
                    }
                lastTime = currentTime;
                accumulator += deltaTime;

                avgFPS[avgFPSIndex++] = deltaTime;
                if (avgFPSIndex >= avgFPSCount) 
                    {
                        avgFPSIndex = 0;
                        fe::time finalAverage;
                        for (int i = 0; i < avgFPSCount; i++)
                            {
                                finalAverage += avgFPS[i];
                            }
                        finalAverage /= avgFPSCount;
                        app.setTitle(fmt::format("fps: {:.0f} : particle count: {}", 1.f / finalAverage.asSeconds(), particles.particleCount()).c_str());
                    }

                if (globals::g_inputs->keyState("debug", "close") == inputHandler::inputState::PRESS)
                    {
                        app.close();
                    }

                if (globals::g_inputs->mouseState("debug", "left") == inputHandler::inputState::PRESS)
                    {
                        /*for (int i = 0; i < 10000; i++)
                            {
                                particles.addParticle({ 500.f * fe::randomNormal(), 500.f}, {10.f * (1.f - 2.f * fe::randomNormal()), 150.f}, particleAccelerationCurveType::NONE, fe::seconds(1.f + 2.f * fe::randomNormal()));
                            }*/
                    }

                particles.handleDestruction();
                graphicsSystem.handleDestruction();

                game.preUpdate();
                game.update();

                while (accumulator >= simulationRate)
                    {
                        accumulator -= simulationRate;
                        game.fixedUpdate(static_cast<float>(deltaTime.asSeconds()));
                    }

                game.postUpdate();

                game.preDraw();

                game.draw(graphicsEngine);

                game.postDraw();

                app.pollEvents();
            }

        return 0;
    }