#ifndef _NIMA_WRITER_HPP
#define _NIMA_WRITER_HPP

#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>
#include <vector>
#include <rapidjson/document.h>

namespace nima
{
    class Writer
    {
        protected:
            rapidjson::Document& m_Config;

        public:
            Writer(rapidjson::Document& config) : m_Config(config)
            {
            }
            ~Writer()
            {
            }

            virtual void screenshot(const char* filename) = 0;
            // virtual std::vector<char*> imageSequence() = 0;
    };
}

#endif