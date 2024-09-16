#include "aupch.h"
#include "Shader.h"

#include "Aurora/Core/Log.h"

namespace Aurora {
    void ShaderLibrary::Add(const std::string& name, const std::shared_ptr<Shader>& shader) {
        if(Exists(name)) {
            AU_CORE_WARN("Shader already exists!");
            return;
        }
        m_Shaders[name] = shader;
    }

    void ShaderLibrary::Add(const std::shared_ptr<Shader>& shader) {
        auto& name = shader->GetName();
        Add(name, shader);
    }

    //std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& filepath)
    //{
    //	auto shader = Shader::Create(filepath);
    //	Add(shader);
    //	return shader;
    //}

    //std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
    //{
    //	auto shader = Shader::Create(filepath);
    //	Add(name, shader);
    //	return shader;
    //}

    std::shared_ptr<Shader> ShaderLibrary::Get(const std::string& name) {
        if (!Exists(name)) {
            AU_CORE_WARN("Shader do not exists!");
            return nullptr;
        }
        return m_Shaders[name];
    }

    bool ShaderLibrary::Exists(const std::string& name) const {
        return m_Shaders.contains(name);
    }

}
