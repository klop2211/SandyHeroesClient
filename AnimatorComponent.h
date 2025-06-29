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

    //setter
    void set_animation_state(AnimationState* value);
    void set_speed_scale(float value);
    void set_is_ignore_root_bone_traslation(bool value);

    //getter
    AnimationState* animation_state() const;

private:
    std::unique_ptr<AnimationState> animation_state_;
    
	float track_index_ = -1;         //���� ������� Ʈ�� �ε���
	float before_track_index_ = -1;  //���� Ʈ�� �ε���

    //������ �ִϸ��̼� ���
    std::vector<AnimationTrack> animation_tracks_;
    std::vector<std::string> frame_names_;
    std::string root_bone_name_;

	std::vector<XMFLOAT4X4> animated_tramsforms_; //�ִϸ��̼��� ����� ������ transform ��ĵ�
    float change_time_ = 0.f;       // �ִϸ��̼��� ��������ϰ� ���� �ð�(��)
	float max_change_time_ = 0.5f;  // �ִϸ��̼��� ����Ǵµ� �ɸ��� �ð�(��)


    //�ִϸ��̼��� ����Ǵ� ����
    std::vector<Object*> bone_frames_;
    Object* root_bone_frame_;
    bool is_attached_bone_frames_ = false;

    //Ȱ��ȭ �� ��Ų�޽��� ��Ʈ���� �������� translation ���� root_node�� �����
    bool is_root_motion_animation_ = false; 
    bool is_ignore_root_bone_traslation_ = false; // true�� root_bone�� ����Ǵ� �ִϸ��̼��� translation ���� ���õ�

    //�ִϸ��̼� ��� �ӵ�
    float speed_scale_ = 1.f;
};

