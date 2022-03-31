#include "graphics/window.hpp"
#include "graphics/shader.hpp"
#include "graphics/camera.hpp"
#include "graphics/texture.hpp"
#include "graphics/transformable.hpp"
#include "graphics/graphicsEngine.hpp"

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

        fe::randomImpl c_generator{};
        c_generator.startUp();
        c_generator.seed(1337);

        window app(1280, 720, "Logistic Simulator");
        glfwSetInputMode(app.getWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

        graphicsSystem graphicsSystem;
        graphicsEngine graphicsEngine(app, graphicsSystem);

        globals::g_graphicsSystem = &graphicsSystem;
        globals::g_graphicsEngine = &graphicsEngine;

        inputHandler c_inputHandler(app.getWindow(), "inputs.ini");
        globals::g_inputs = &c_inputHandler;

        c_inputHandler.addDefaultInput("debug", "close", GLFW_KEY_ESCAPE);

        c_inputHandler.save("inputs.ini");

        constexpr fe::time simulationRate = fe::seconds(1.f / 60.f);
        constexpr fe::time maxDeltaTime = fe::seconds(3);
        fe::clock runClock;
        fe::time lastTime = runClock.getTime();
        fe::time accumulator;

        gameStateMachine game;
        game.push<breakout>();

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

                if (globals::g_inputs->keyState("debug", "close") == inputHandler::inputState::PRESS)
                    {
                        app.close();
                    }

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