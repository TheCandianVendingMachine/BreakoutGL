// random.hpp
// allows for easy randomization of items
#pragma once
#include "typeDefines.hpp"

namespace fe
    {
        class randomImpl
            {
                private:
                    fe::uint64 stateA;

                    static randomImpl *m_instance;

                public:
                    void startUp();

                    void seed(fe::uint64 seed);
                    void randomSeed();

                    static randomImpl &get();
                    // generate a number between [min, max]
                    fe::uint64 generate(fe::uint64 min, fe::uint64 max);
                    fe::uint64 generate();
            };

        // returns random number between 0 and 1
        float randomNormal();
        // returns random number between range
        float randomNormal(float min, float max);

        fe::uint64 random(fe::uint64 min, fe::uint64 max);
        fe::uint64 random();
    }