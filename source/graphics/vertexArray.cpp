#include "vertexArray.hpp"
#include "glad/glad.h"
#include <spdlog/spdlog.h>
#include <utility>

vertexArray::vertexArray() :
    m_usage(GL_STATIC_DRAW)
    {
        glGenVertexArrays(1, &m_vao);
    }

vertexArray::vertexArray(unsigned int usage) :
    m_usage(usage)
    {
        glGenVertexArrays(1, &m_vao);
    }

vertexArray::vertexArray(const vertexArray &rhs)
    {
        *this = rhs;
    }

vertexArray::vertexArray(vertexArray &&rhs)
    {
        *this = std::move(rhs);
    }

vertexArray::~vertexArray()
    {
        /*if (m_ownBuffers)
            {
                glDeleteBuffers(1, &m_ebo);
                glDeleteBuffers(1, &m_vbo);
            }
        glDeleteVertexArrays(1, &m_vao);*/
    }

vertexArray &vertexArray::operator=(const vertexArray &rhs)
    {
        if (&rhs != this)
            {
                m_vao = rhs.m_vao;
                m_vbo = rhs.m_vbo;
                m_ebo = rhs.m_ebo;

                m_vertexCount = rhs.m_vertexCount;
                m_indexCount = rhs.m_indexCount;

                m_ownBuffers = rhs.m_ownBuffers;
            }

        return *this;
    }

vertexArray &vertexArray::operator=(vertexArray &&rhs)
    {
        if (&rhs != this)
            {
                m_vao = std::move(rhs.m_vao);
                m_vbo = std::move(rhs.m_vbo);
                m_ebo = std::move(rhs.m_ebo);

                m_vertexCount = std::move(rhs.m_vertexCount);
                m_indexCount = std::move(rhs.m_indexCount);

                m_ownBuffers = std::move(rhs.m_ownBuffers);
            }

        return *this;
    }

void vertexArray::bindVertices(const std::vector<vertex> &vertices)
    {
        m_vertexCount = static_cast<unsigned int>(vertices.size());

        if (m_vbo == 0)
            {
                glGenBuffers(1, &m_vbo);
            }

        glBindVertexArray(m_vao);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, m_vertexCount * sizeof(vertex), vertices.data(), m_usage);

        glBindVertexArray(0);

        m_ownBuffers = true;
    }

void vertexArray::bindVertices(unsigned int vbo, unsigned int vertexCount)
    {
        if (m_vbo != 0)
            {
                spdlog::warn("Binding VBO to vertexArray with an already bound VBO");
            }

        m_vertexCount = vertexCount;
        m_vbo = vbo;
        m_ownBuffers = false;
    }

void vertexArray::bindIndices(const std::vector<unsigned int> &indices)
    {
        m_indexCount = static_cast<unsigned int>(indices.size());

        if (m_ebo == 0)
            {
                glGenBuffers(1, &m_ebo);
            }

        glBindVertexArray(m_vao);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexCount * sizeof(unsigned int), indices.data(), m_usage);

        glBindVertexArray(0);

        m_ownBuffers = true;
    }

void vertexArray::bindIndices(unsigned int ebo, unsigned int indexCount)
    {
        if (m_ebo != 0)
            {
                spdlog::warn("Binding EBO to vertexArray with an already bound EBO");
            }

        m_indexCount = indexCount;
        m_ebo = ebo;
        m_ownBuffers = false;
    }

void vertexArray::use(vertex::attributes attributes) const
    {
        if (m_vbo == 0)
            {
                spdlog::error("Cannot use attributes without a VBO bound");
            }

        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

        if ((attributes & vertex::attributes::POSITION) == vertex::attributes::POSITION)
            {
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void*>(offsetof(vertex, position)));
                glEnableVertexAttribArray(0);
            }

        if ((attributes & vertex::attributes::NORMAL) == vertex::attributes::NORMAL)
            {
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void*>(offsetof(vertex, normal)));
            }

        if ((attributes & vertex::attributes::TANGENT) == vertex::attributes::TANGENT)
            {
                glEnableVertexAttribArray(2);
                glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void*>(offsetof(vertex, tangent)));
            }

        if ((attributes & vertex::attributes::TEXTURE) == vertex::attributes::TEXTURE)
            {
                glEnableVertexAttribArray(3);
                glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void*>(offsetof(vertex, textureCoordinate)));
            }

        if ((attributes & vertex::attributes::COLOUR) == vertex::attributes::COLOUR)
            {
                glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void*>(offsetof(vertex, colour)));
                glEnableVertexAttribArray(4);
            }

        glBindVertexArray(0);
    }

