#pragma once

//class Scene;

//컷신을 녹화하는 클래스
// 녹화시 씬에서 사용하는 메인 카메라의 월드행렬을 저장
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

