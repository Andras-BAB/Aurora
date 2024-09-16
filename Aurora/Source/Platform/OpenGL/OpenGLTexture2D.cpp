#include "aupch.h"
#include "OpenGLTexture2D.h"

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
    
    OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specification) {
    }

    OpenGLTexture2D::OpenGLTexture2D(const std::string& path) {
    }

    OpenGLTexture2D::~OpenGLTexture2D() {
    }

    void OpenGLTexture2D::SetData(void* data, uint32_t size) {
    }

    void OpenGLTexture2D::Bind(uint32_t slot) const {
    }
}
