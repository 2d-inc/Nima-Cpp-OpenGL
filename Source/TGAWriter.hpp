#ifndef _NIMA_TGAWRITER_HPP
#define _NIMA_TGAWRITER_HPP

#include "./Writer.hpp"

namespace nima
{
    class TGAWriter : Writer
    {
        private:
            char *readPixelData()
            {
                int w = m_Config["width"].GetInt();
                int h = m_Config["height"].GetInt();

                int nSize = w * h * 3;
                char *dataBuffer = (char *)malloc(nSize);

                if (!dataBuffer)
                {
                    printf("NO MORE MEMORY!\n");
                    return 0;
                }

                GLuint fbo, renderBuf;
                glGenFramebuffers(1, &fbo);
                glGenRenderbuffers(1, &renderBuf);

                glBindRenderbuffer(GL_RENDERBUFFER, renderBuf);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_BGR, w, h);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
                glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuf);

                glPixelStorei(GL_PACK_ALIGNMENT, 1);

                glReadPixels((GLint)0, (GLint)0, (GLint)w, (GLint)h, GL_BGR, GL_UNSIGNED_BYTE, dataBuffer);

                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
                glDeleteFramebuffers(1, &fbo);
                glDeleteRenderbuffers(1, &renderBuf);

                return dataBuffer;
            }

        public:
            TGAWriter(rapidjson::Document& config) : Writer(config)
            {
            }
            ~TGAWriter()
            {
            }
            // std::vector<char*> imageSequence();
            
            void screenshot(const char* filename)
            {
                FILE* fp = fopen(filename, "wb");
                if(!fp)
                {
                    printf("ERROR CREATING FILE %s\n", filename);
                    return;
                }

                int w = m_Config["width"].GetInt();
                int h = m_Config["height"].GetInt();

                char* dataBuffer = readPixelData();
                if(!dataBuffer)
                {
                    printf("NO DATA AVAILABE?\n");
                    fclose(fp);
                    return;
                }

                unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                unsigned char v0 = static_cast<unsigned char>(w % 256);
                unsigned char v1 = static_cast<unsigned char>(w / 256);
                unsigned char v2 = static_cast<unsigned char>(h % 256);
                unsigned char v3 = static_cast<unsigned char>(h / 256);
                unsigned char header[6] = { v0, v1, v2, v3, 24, 0 };

                fwrite(TGAheader, sizeof(unsigned char), 12, fp);
                fwrite(header, sizeof(unsigned char), 6, fp);
                fwrite(dataBuffer, sizeof(GLubyte), w*h*3, fp);

                fclose(fp);
                free(dataBuffer);
            }   
    };
}


#endif