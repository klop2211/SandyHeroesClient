#pragma once
#include "MeshComponent.h"
class SkinnedMeshComponent :
    public MeshComponent
{
public:
    SkinnedMeshComponent(Object* owner, Mesh* mesh);
    SkinnedMeshComponent(const SkinnedMeshComponent& other);
    SkinnedMeshComponent& operator=(const SkinnedMeshComponent& rhs);

    virtual Component* GetCopy() override;

    virtual void UpdateConstantBuffer(FrameResource* current_frame_resource, int cb_index) override;

    // skinned mesh�� bone names ������ ����Ͽ� bone_frames_ �迭�� �����Ѵ�.
    void AttachBoneFrames(const std::vector<std::string>& bone_names);

private:
    std::vector<Object*> bone_frames_;

    bool is_attached_bone_frames_ = false;

};

