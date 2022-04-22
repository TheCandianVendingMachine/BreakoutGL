// soundSystem.hpp
// A high-level wrapper around FMOD. Allows the playing of audio through events
#pragma once
#include <fmod_studio.hpp>
#include <vector>
#include <robin_hood.h>
#include <plf_colony.h>
#include <queue>
#include "str.hpp"

class soundSystem
    {
        private:
            FMOD::Studio::System* m_system = nullptr;

            unsigned int m_bankID = 0;
            robin_hood::unordered_map<unsigned int, FMOD::Studio::Bank*> m_loadedBanks;

            robin_hood::unordered_map<fe::str, FMOD::Studio::EventDescription*> m_descriptions;

            plf::colony<FMOD::Studio::EventInstance*> m_playingInstances;

            __forceinline void verifyImportant(FMOD_RESULT result) const; // crashes on bad result
            __forceinline bool verifyCasual(FMOD_RESULT result) const; // returns false on bad result

            void loadBank(const char *bank);
            FMOD::Studio::EventInstance* createInstance(const char* event);

            template<typename T>
            constexpr void getHexFromData(T data, char *out, int *index) const
                {
                    constexpr char hexLookupTable[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
                    T quotient = data;
                    unsigned int startIndex = *index;
                    do {
                        T remainder = quotient % 16;
                        quotient /= 16;
                        out[(*index)++] = hexLookupTable[remainder];
                    } while (quotient > 0);

                    if (((*index) - startIndex) % 2 != 0)
                        {
                            out[(*index)++] = '0';
                        }

                    unsigned int endIndex = startIndex + ((*index) - startIndex) / 2;
                    for (unsigned int i = startIndex; i < endIndex; i++)
                        {
                            int swapIndex = ((*index) - (i - startIndex) - 1);
                            char temp = out[i];
                            char swap = out[swapIndex];
                            out[i] = out[swapIndex];
                            out[swapIndex] = temp;
                        }
                }

            constexpr fe::str getStrFromGUID(FMOD_GUID *guid) const
                {
                    char out[37] = "\0";
                    int index = 0;
                    getHexFromData(guid->Data1, out, &index);
                    out[index++] = '-';
                    getHexFromData(guid->Data2, out, &index);
                    out[index++] = '-';
                    getHexFromData(guid->Data3, out, &index);
                    out[index++] = '-';
                    getHexFromData(guid->Data4[0], out, &index);
                    getHexFromData(guid->Data4[1], out, &index);
                    out[index++] = '-';
                    getHexFromData(guid->Data4[2], out, &index);
                    getHexFromData(guid->Data4[3], out, &index);
                    getHexFromData(guid->Data4[4], out, &index);
                    getHexFromData(guid->Data4[5], out, &index);
                    getHexFromData(guid->Data4[6], out, &index);
                    getHexFromData(guid->Data4[7], out, &index);
                    out[index++] = '\0';

                    return fe::hash(out, 5381); // constexpr version
                }

        public:
            soundSystem();
            ~soundSystem();

            void update();

            void play(const char *event);
    };

namespace globals
    {
        extern soundSystem *g_audioSystem;
    }
