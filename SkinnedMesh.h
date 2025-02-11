#pragma once
#include "Mesh.h"
class SkinnedMesh :
    public Mesh
{
public:
    virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;

    virtual void Render(ID3D12GraphicsCommandList* command_list,
        FrameResourceManager* frame_resource_manager, DescriptorManager* descriptor_manager) override;

    void LoadSkinnedMeshFromFile(std::ifstream& file);

private:
    int bones_per_vertex_ = 4;

    std::vector<std::string> bone_names_;

    std::vector<XMFLOAT4X4> bone_offsets_;
    std::vector<XMINT4> bone_indices_;
    std::vector<XMFLOAT4> bone_weights_;

};

