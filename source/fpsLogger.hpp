// fpsLogger.hpp
// Calculates FPS based off of average frametime
#pragma once
#include "clock.hpp"
#include "time.hpp"

template<unsigned int averageCount>
class fpsLogger
    {
        private:
            fe::clock timer;
            fe::time fpsCounter[averageCount];
            unsigned int currentCounterIndex = 0;

        public:
            void newFrame()
                {
                    currentCounterIndex = (currentCounterIndex + 1) % averageCount;
                    fpsCounter[currentCounterIndex] = timer.getTime();

                    timer.restart();
                }

            double getFPS() const
                {
                    fe::time averageFPS;
                    for (unsigned int i = 0; i < currentCounterIndex; i++)
                        {
                            averageFPS += fpsCounter[currentCounterIndex];
                        }
                    double frameTime = averageFPS.asSeconds();
                    return static_cast<double>(currentCounterIndex) / frameTime;
                }

    };
