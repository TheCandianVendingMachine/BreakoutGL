#include "image.hpp"
#include "stb_image.h"
#include "stb_image_write.h"
#include <cstring>
#include <cctype>
#include <utility>

image::extensions image::getExtension(const char *file) const
    {
        extensions potentialExtensions = extensions::PNG | extensions::BMP | extensions::TGA | extensions::JPG;

        // reversed because we are checking backwards
        char extensionStrings[5][4] = {
            { 'g', 'n', 'p', '.' },
            { 'p', 'm', 'b', '.' },
            { 'a', 'g', 't', '.' },
            { 'g', 'p', 'j', '.' },
            { 'g', 'e', 'p', 'j' }
        };

        std::size_t length = std::strlen(file);
        for (std::size_t i = length - 1; i >= length - 4; i--)
            {
                if (file[i] == '.')
                    {
                        // end of extension
                        break;
                    }
                
                char lowered = std::tolower(file[i]);
                if (((potentialExtensions & extensions::PNG) == extensions::PNG) && lowered != extensionStrings[0][length - i - 1])
                    {
                        potentialExtensions = static_cast<extensions>(potentialExtensions ^ extensions::PNG);
                    }
                if (((potentialExtensions & extensions::BMP) == extensions::BMP) && lowered != extensionStrings[1][length - i - 1])
                    {
                        potentialExtensions = static_cast<extensions>(potentialExtensions ^ extensions::BMP);
                    }
                if (((potentialExtensions & extensions::TGA) == extensions::TGA) && lowered != extensionStrings[2][length - i - 1])
                    {
                        potentialExtensions = static_cast<extensions>(potentialExtensions ^ extensions::TGA);
                    }
                if (((potentialExtensions & extensions::JPG) == extensions::JPG) && lowered != extensionStrings[3][length - i - 1] && lowered != extensionStrings[4][length - i])
                    {
                        potentialExtensions = static_cast<extensions>(potentialExtensions ^ extensions::JPG);
                    }

                if (potentialExtensions == extensions::NO_EXTENSION)
                    {
                        potentialExtensions = extensions::BMP;
                        break;
                    }
            }

        return potentialExtensions;
    }

void image::writePixelsToCharBuffer(std::vector<unsigned char> &buffer, unsigned int channels) const
    {
        for (const auto &pixel : m_pixels)
            {
                // copy to buffer pixel values that match the channel count
                // memory alignment issues can fuck this up? maybe??
                for (unsigned int i = 0; i < channels; i++)
                    {
                        unsigned char texel = *((&pixel.r) + i);
                        buffer.push_back(texel);
                    }
            }
    }

void image::loadPixelData(unsigned char *pixels)
    {
        // channels is channels that are availible in the image, while we will always get c_channels from the pointer
        m_pixels.resize(m_width * m_height);
        for (unsigned int i = 0; i < m_width * m_height; i++)
            {
                m_pixels[i].r = pixels[i * m_channels + 0];
                m_pixels[i].g = pixels[i * m_channels + 1];
                m_pixels[i].b = pixels[i * m_channels + 2];
                m_pixels[i].a = pixels[i * m_channels + 3];
            }
    }

image::image(unsigned int channels) :
    m_channels(channels)
    {
    }

image::image(unsigned int width, unsigned int height, unsigned int channels) :
    m_channels(channels)
    {
        create(width, height);
    }

void image::setChannels(unsigned int channels)
    {
        m_channels = channels;
    }

unsigned int image::getChannels() const
    {
        return m_channels;
    }

void image::create(unsigned int width, unsigned int height)
    {
        m_pixels.resize(width * height);

        m_width = width;
        m_height = height;
    }

void image::clear(unsigned char r, unsigned char g, unsigned char b)
    {
        clear(r, g, b, 255);
    }

void image::clear(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    {
        std::fill(m_pixels.begin(), m_pixels.end(), rgba{ r, g, b, a });
    }

void image::set(unsigned int x, unsigned int y, rgba pixel)
    {
        set(x, y, pixel.r, pixel.g, pixel.b, pixel.a);
    }

void image::set(unsigned int x, unsigned int y, unsigned char r)
    {
        set(x, y, r, r, r, 255);
    }

void image::set(unsigned int x, unsigned int y, unsigned char r, unsigned char g)
    {
        set(x, y, r, g, g, 255);
    }

void image::set(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b)
    {
        set(x, y, r, g, b, 255);
    }

void image::set(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    {
        m_pixels[x + y * m_width].r = r;
        if (m_channels >= 2) { m_pixels[x + y * m_width].g = g; }
        if (m_channels >= 3) { m_pixels[x + y * m_width].b = b; }
        if (m_channels >= 4) { m_pixels[x + y * m_width].a = a; }
    }

image::rgba image::get(unsigned int x, unsigned int y) const
    {
        x = std::min(m_width - 1, std::max(0u, x));
        y = std::min(m_height - 1, std::max(0u, y));
        return m_pixels[x + y * m_width];
    }

void image::saveToFile(const char *file) const
    {
        extensions extension = getExtension(file);

        std::vector<unsigned char> pixels;
        writePixelsToCharBuffer(pixels, m_channels);

        // 4 channels since we always encode as RGBA
        switch (extension)
            {
                case extensions::PNG:
                    stbi_write_png(file, m_width, m_height, m_channels, pixels.data(), 0);
                    break;
                case extensions::BMP:
                    stbi_write_bmp(file, m_width, m_height, m_channels, pixels.data());
                    break;
                case extensions::TGA:
                    stbi_write_tga(file, m_width, m_height, m_channels, pixels.data());
                    break;
                case extensions::JPG:
                    stbi_write_jpg(file, m_width, m_height, m_channels, pixels.data(), 100);
                    break;
                default:
                    break;
            }
    }

void image::saveToMemory(std::vector<unsigned char> &buffer, extensions extension) const
    {
        std::vector<unsigned char> pixels;
        writePixelsToCharBuffer(pixels, m_channels);

        int bufferSizeCopied = 0;

        auto writeFunc = [] (void *context, void *data, int size) {
            std::vector<unsigned char> &buffer = *static_cast<std::vector<unsigned char>*>(context);

            unsigned char *dataChar = static_cast<unsigned char *>(data);
            for (int i = 0; i < size; i++)
                {
                    buffer.push_back(*(dataChar + i));
                }
        };

        // 4 channels since we always encode as RGBA
        switch (extension)
            {
                case extensions::PNG:
                    stbi_write_png_to_func(writeFunc, &buffer, m_width, m_height, m_channels, pixels.data(), 0);
                    break;
                case extensions::BMP:
                    stbi_write_bmp_to_func(writeFunc, &buffer, m_width, m_height, m_channels, pixels.data());
                    break;
                case extensions::TGA:
                    stbi_write_tga_to_func(writeFunc, &buffer, m_width, m_height, m_channels, pixels.data());
                    break;
                case extensions::JPG:
                    stbi_write_jpg_to_func(writeFunc, &buffer, m_width, m_height, m_channels, pixels.data(), 100);
                    break;
                default:
                    break;
            }
    }

void image::loadFromFile(const char *file)
    {
        int width, height, channels;

        stbi_uc *pixelData = stbi_load(file, &width, &height, &channels, m_channels);
        if (!pixelData) { return; }
        m_width = width;
        m_height = height;
        loadPixelData(pixelData);

        stbi_image_free(pixelData);
    }

void image::loadFromMemory(std::vector<unsigned char> &buffer)
    {
        int width, height, channels;
        stbi_uc *pixelData = stbi_load_from_memory(buffer.data(), buffer.size() * sizeof(unsigned char), &width, &height, &channels, m_channels);
        if (!pixelData) { return; }
        m_width = width;
        m_height = height;
        loadPixelData(pixelData);

        stbi_image_free(pixelData);
    }

unsigned int image::getWidth() const
    {
        return m_width;
    }

unsigned int image::getHeight() const
    {
        return m_height;
    }

const std::vector<image::rgba> &image::getPixelData() const
    {
        return m_pixels;
    }

const std::size_t image::getBufferSize() const
    {
        return m_pixels.size() * m_channels * sizeof(unsigned char);
    }
