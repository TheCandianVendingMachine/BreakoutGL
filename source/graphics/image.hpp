// image.hpp
// An image whose pixel data can be accessed on the CPU. Use a texture if you want something on the GPU
#pragma once
#include <vector>

class image
    {
        public:
            struct rgba
                {
                    unsigned char r = 0;
                    unsigned char g = 0;
                    unsigned char b = 0;
                    unsigned char a = 255;
                };

            enum class extensions : unsigned char
                {
                    NO_EXTENSION = 0,
                    PNG = 1 << 0,
                    BMP = 1 << 1,
                    TGA = 1 << 2,
                    JPG = 1 << 3,
                };

        private:
            friend extensions operator|(const extensions &lhs, const extensions &rhs) { return static_cast<extensions>(static_cast<unsigned char>(lhs) | static_cast<unsigned char>(rhs)); }
            friend extensions operator&(const extensions &lhs, const extensions &rhs) { return static_cast<extensions>(static_cast<unsigned char>(lhs) & static_cast<unsigned char>(rhs)); }
            friend extensions operator^(const extensions &lhs, const extensions &rhs) { return static_cast<extensions>(static_cast<unsigned char>(lhs) ^ static_cast<unsigned char>(rhs)); }

            extensions getExtension(const char *file) const;

            std::vector<rgba> m_pixels;

            unsigned int m_channels = 4;
            unsigned int m_width = 0;
            unsigned int m_height = 0;

            void writePixelsToCharBuffer(std::vector<unsigned char> &buffer, unsigned int channels) const;
            void loadPixelData(unsigned char *pixels);

        public:
            image(unsigned int channels = 4);
            image(unsigned int width, unsigned int height, unsigned int channels = 4);

            void setChannels(unsigned int channels);
            unsigned int getChannels() const;

            void create(unsigned int width, unsigned int height);
            void clear(unsigned char r, unsigned char g, unsigned char b);
            void clear(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

            void set(unsigned int x, unsigned int y, rgba pixel);
            void set(unsigned int x, unsigned int y, unsigned char r);
            void set(unsigned int x, unsigned int y, unsigned char r, unsigned char g);
            void set(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b);
            void set(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
            rgba get(unsigned int x, unsigned int y) const;

            void saveToFile(const char *file) const;
            void saveToMemory(std::vector<unsigned char> &buffer, extensions extension) const;

            void loadFromFile(const char *file);
            void loadFromMemory(std::vector<unsigned char> &buffer);

            unsigned int getWidth() const;
            unsigned int getHeight() const;

            const std::vector<rgba> &getPixelData() const;

            const std::size_t getBufferSize() const;

    };
