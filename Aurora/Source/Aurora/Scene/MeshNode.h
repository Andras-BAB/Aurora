#pragma once
#include "Components.h"

namespace Aurora {
    
    class MeshNode {
    public:
        virtual ~MeshNode() = default;

        void AddChild(const std::shared_ptr<MeshNode>& child);

    private:
        UUID m_NodeID;
        std::string m_Name;
        std::vector<std::shared_ptr<MeshNode>> m_Children;
    };
}
