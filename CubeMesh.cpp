#include "stdafx.h"
#include "CubeMesh.h"
#include "Shader.h"

CubeMesh::CubeMesh(const XMFLOAT4& color)
{
	shader_type_ = (int)ShaderType::kColor;

	// 정점 8개
	positions_.reserve(8);
	colors_.reserve(8);

	// Left Top Front 
	positions_.emplace_back(-1, 1, -1);  //0
	// R T F
	positions_.emplace_back(1, 1, -1);   //1
	// L B F
	positions_.emplace_back(-1, -1, -1); //2
	// R B F
	positions_.emplace_back(1, -1, -1);  //3
	// L T B
	positions_.emplace_back(-1, 1, 1);   //4
	// R T B
	positions_.emplace_back(1, 1, 1);    //5
	// L B B
	positions_.emplace_back(-1, -1, 1);  //6
	// R B B
	positions_.emplace_back(1, -1, 1);   //7

	for (int i = 0; i < colors_.capacity(); ++i)
	{
		colors_.emplace_back(color);
	}

	//subset 1개, 인덱스 36개
	indices_array_.resize(1);
	auto& index_buffer = indices_array_.back();

	index_buffer.reserve(36);

	//front
	index_buffer.emplace_back(0);
	index_buffer.emplace_back(1);
	index_buffer.emplace_back(3);
	index_buffer.emplace_back(0);
	index_buffer.emplace_back(3);
	index_buffer.emplace_back(2);

	//back
	index_buffer.emplace_back(5);
	index_buffer.emplace_back(6);
	index_buffer.emplace_back(7);
	index_buffer.emplace_back(5);
	index_buffer.emplace_back(4);
	index_buffer.emplace_back(6);

	//left
	index_buffer.emplace_back(4);
	index_buffer.emplace_back(0);
	index_buffer.emplace_back(6);
	index_buffer.emplace_back(0);
	index_buffer.emplace_back(2);
	index_buffer.emplace_back(6);

	//right
	index_buffer.emplace_back(1);
	index_buffer.emplace_back(5);
	index_buffer.emplace_back(3);
	index_buffer.emplace_back(5);
	index_buffer.emplace_back(7);
	index_buffer.emplace_back(3);

	//top
	index_buffer.emplace_back(0);
	index_buffer.emplace_back(5);
	index_buffer.emplace_back(1);
	index_buffer.emplace_back(0);
	index_buffer.emplace_back(4);
	index_buffer.emplace_back(5);

	//bottom
	index_buffer.emplace_back(6);
	index_buffer.emplace_back(3);
	index_buffer.emplace_back(7);
	index_buffer.emplace_back(6);
	index_buffer.emplace_back(2);
	index_buffer.emplace_back(3);

}
