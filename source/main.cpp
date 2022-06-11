#include "graphics/window.hpp"
#include "graphics/shader.hpp"
#include "graphics/camera.hpp"
#include "graphics/texture.hpp"
#include "graphics/transformable.hpp"
#include "graphics/graphicsEngine.hpp"
#include "graphics/particles/particleRenderer.hpp"

#include "fpsLogger.hpp"
#include "threadPool.hpp"
#include "random.hpp"
#include "clock.hpp"
#include "time.hpp"
#include "str.hpp"
#include "inputHandler.hpp"
#include "messaging/globalEventHandler.hpp"

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "ecs/graphicsSystem.hpp"

#include "game/states/gameStateMachine.hpp"
#include "game/states/breakout.hpp"

#include "sound/soundSystem.hpp"

#include "ui/widgetManager.hpp"
#include "ui/widgetSerializer.hpp"
#include "ui/font/fontEngine.hpp"

int main()
    {
        spdlog::set_level(spdlog::level::debug);
        globals::commonThreadPool threadPool;
        globals::g_threadPool = &threadPool;

        window app(500, 1000, "Breakout");
        glfwSetInputMode(app.getWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

        inputHandler c_inputHandler(app.getWindow(), "inputs.ini");
        globals::g_inputs = &c_inputHandler;

        globalEventHandler globalEvents;
        globals::g_globalEventHandler = &globalEvents;

        soundSystem audioEngine;
        globals::g_audioSystem = &audioEngine;

        fe::randomImpl c_generator{};
        c_generator.startUp();
        c_generator.seed(1337);

        particleRenderer particles;
        globalEvents.subscribe("spawn particles", [&particles] (message &m, int) {
            int count = 0;
            glm::vec2 initialPosition = { 0.f, 0.f };
            glm::vec2 initialVelocity = { 0.f, 0.f };
            glm::vec2 randomPositionOffset = { 0.f, 0.f };
            glm::vec2 randomVelocityOffset = { 0.f, 0.f };
            std::int64_t lifespan = 0;
            float size = 0.f;
            int accelerationType = 0;

            m.arguments[0].cast(count);
            m.arguments[1].cast(initialPosition);
            m.arguments[2].cast(initialVelocity);
            m.arguments[3].cast(randomPositionOffset);
            m.arguments[4].cast(randomVelocityOffset);
            m.arguments[5].cast(lifespan);
            m.arguments[6].cast(size);
            m.arguments[7].cast(accelerationType);

            for (int i = 0; i < count; i++)
                {
                    glm::vec2 position = initialPosition + glm::vec2(randomPositionOffset.x * (1.f - 2.f * fe::randomNormal()), randomPositionOffset.y * (1.f - 2.f * fe::randomNormal()));
                    glm::vec2 velocity = initialVelocity + glm::vec2(randomVelocityOffset.x * (1.f - 2.f * fe::randomNormal()), randomVelocityOffset.y * (1.f - 2.f * fe::randomNormal()));

                    particles.addParticle(position, velocity, static_cast<particleAccelerationCurveType>(accelerationType), fe::time(lifespan), size);
                }
        });

        
        widgetManager widgets({ app.width, app.height });
        fontEngine fonts;
        fontEngine::fontReference font = fonts.load("CODE.otf", "default");

        graphicsSystem graphicsSystem;
        graphicsEngine graphicsEngine(app, graphicsSystem, particles, widgets);

        widgetSerializer::loadFromFile(widgets, "guiTest.csv");

        widgets.subscribe("continue game", [](message& m, int) { spdlog::info("continue"); });
        widgets.subscribe("goto settings", [](message& m, int) { spdlog::info("settings"); });
        widgets.subscribe("quit game", [](message& m, int) { spdlog::info("quit"); });

        globals::g_graphicsSystem = &graphicsSystem;
        globals::g_graphicsEngine = &graphicsEngine;

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

        fpsLogger<16> fpsCounter;

        while (app.isOpen())
            {
                fpsCounter.newFrame();

                fe::time currentTime = runClock.getTime();
                fe::time deltaTime = currentTime - lastTime;
                if (deltaTime >= maxDeltaTime)
                    {
                        deltaTime = maxDeltaTime;
                    }
                lastTime = currentTime;
                accumulator += deltaTime;

                if (globals::g_inputs->keyState("debug", "close") == inputHandler::inputState::PRESS)
                    {
                        app.close();
                    }

                widgets.update();

                audioEngine.update();

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