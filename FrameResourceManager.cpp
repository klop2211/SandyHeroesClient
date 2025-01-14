#include "stdafx.h"
#include "FrameResourceManager.h"
#include "FrameResource.h"

void FrameResourceManager::ResetFrameResources(ID3D12Device* device, UINT cb_pass_count, 
	UINT cb_object_count, UINT cb_skinned_mesh_object_count)
{
    frame_resources_.clear();
    frame_resources_.reserve(kFrameCount);
    curr_frame_resource_index_ = 0;

    for (int i = 0; i < kFrameCount; ++i)
    {
        frame_resources_.push_back(std::make_unique<FrameResource>(
            device, cb_pass_count, (UINT)cb_object_count, (UINT)cb_skinned_mesh_object_count));
    }

    curr_frame_resource_ = frame_resources_[curr_frame_resource_index_].get();
}
