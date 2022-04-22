// graphicsEngine.hpp
// High level API to render arbitrary VAO's with associated transforms
#pragma once
#include <type_traits>
#include "shader.hpp"
#include "particles/particleRenderer.hpp"

class camera;
class window;
class graphicsSystem;
class tilemapSystem;
class widgetManager;
class graphicsEngine
    {
        private:
            tilemapSystem *m_tilemapSystem = nullptr;
            graphicsSystem &m_graphicsSystem;
            particleRenderer &m_particleSystem;
            widgetManager &m_widgetManager;
            window &m_window;

            shader m_tilemapShader;
            shader m_2dShader;

            unsigned int m_screenWidth = 0;
            unsigned int m_screenHeight = 0;

            void createFramebuffers();

        public:
            enum class drawFlags
                {
                    NONE = 0,
                    ALL = 1 << 1,
                    COUNT
                };
            friend drawFlags operator&(const drawFlags &lhs, const drawFlags &rhs) { return static_cast<drawFlags>(static_cast<std::underlying_type<drawFlags>::type>(lhs) & static_cast<std::underlying_type<drawFlags>::type>(rhs)); }
            friend drawFlags operator|(const drawFlags &lhs, const drawFlags &rhs) { return static_cast<drawFlags>(static_cast<std::underlying_type<drawFlags>::type>(lhs) | static_cast<std::underlying_type<drawFlags>::type>(rhs)); }
            friend drawFlags operator^(const drawFlags &lhs, const drawFlags &rhs) { return static_cast<drawFlags>(static_cast<std::underlying_type<drawFlags>::type>(lhs) ^ static_cast<std::underlying_type<drawFlags>::type>(rhs)); }
            friend drawFlags &operator&=(drawFlags &lhs, const drawFlags &rhs) { lhs = lhs & rhs; return lhs; }
            friend drawFlags &operator|=(drawFlags &lhs, const drawFlags &rhs) { lhs = lhs | rhs; return lhs; }
            friend drawFlags &operator^=(drawFlags &lhs, const drawFlags &rhs) { lhs = lhs ^ rhs; return lhs; }

            graphicsEngine(window &app, graphicsSystem &graphicsSystem, particleRenderer &particleSystem, widgetManager &widgetManager);
            void draw(const camera &camera, unsigned int texture = 0, drawFlags flags = drawFlags::ALL) const;

            void setTilemap(tilemapSystem *tilemap);
    };

namespace globals
    {
        extern graphicsEngine *g_graphicsEngine;
    }
