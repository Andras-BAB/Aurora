#include "aupch.h"
#include "OpenGLTexture.h"

#include "Aurora/Core/Assert.h"

namespace Aurora {

    namespace Utils {

        static GLenum AuroraImageFormatToGLDataFormat(ImageFormat format) {
            switch (format) {
            case ImageFormat::RGB8:  return GL_RGB;
            case ImageFormat::RGBA8: return GL_RGBA;
            }

            AU_CORE_ASSERT(false);
            return 0;
        }
		
        static GLenum AuroraImageFormatToGLInternalFormat(ImageFormat format) {
            switch (format) {
            case ImageFormat::RGB8:  return GL_RGB8;
            case ImageFormat::RGBA8: return GL_RGBA8;
            }

            AU_CORE_ASSERT(false);
            return 0;
        }

    }
    
    OpenGLTexture::OpenGLTexture(const TextureSpecification& specification) {
    }

    OpenGLTexture::OpenGLTexture(const std::string& path) {
    }

    OpenGLTexture::~OpenGLTexture() {
    }

    void OpenGLTexture::SetData(void* data, uint32_t size) {
    }

    void OpenGLTexture::Bind(uint32_t slot) const {
    }
}
