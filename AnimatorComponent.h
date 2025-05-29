#pragma once
#include "Component.h"
#include "AnimationTrack.h"

class AnimationState;

//������Ʈ�� �ִϸ��̼��� �����ϴ� ������Ʈ
class AnimatorComponent :
    public Component
{
public:
    AnimatorComponent(Object* owner,
        const std::vector<std::unique_ptr<AnimationSet>>& animation_sets,
        const std::vector<std::string>& frame_names,
        const std::string& root_bone_name,
        AnimationState* animation_state);
    
    AnimatorComponent(const AnimatorComponent& other);
    ~AnimatorComponent() {}
    
    virtual Component* GetCopy() override;
    virtual void Update(float elapsed_time) override;

    void AttachBoneFrames();

    void set_animation_state(AnimationState* value);
    AnimationState* animation_state() const;

private:
    std::unique_ptr<AnimationState> animation_state_;
    
    float track_index_ = 0;

    //������ �ִϸ��̼� ���
    std::vector<AnimationTrack> animation_tracks_;
    std::vector<std::string> frame_names_;
    std::string root_bone_name_;

    //�ִϸ��̼��� ����Ǵ� ����
    std::vector<Object*> bone_frames_;
    Object* root_bone_frame_;
    bool is_attached_bone_frames_ = false;

    //Ȱ��ȭ �� ��Ų�޽��� ��Ʈ���� �������� translation ���� root_node�� �����
    bool is_root_motion_animation_ = false; 
};

