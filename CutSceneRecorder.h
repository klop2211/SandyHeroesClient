#pragma once

//class Scene;

//�ƽ��� ��ȭ�ϴ� Ŭ����
// ��ȭ�� ������ ����ϴ� ���� ī�޶��� ��������� ����
class CutSceneRecorder
{
public:
	CutSceneRecorder();
	~CutSceneRecorder();

	void CaptureCameraTransform();
	
	void WriteCutScene();

private:
	//Scene* scene_{ nullptr };



};

