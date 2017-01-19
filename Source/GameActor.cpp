#include "GameActor.hpp"
#include "Graphics/GraphicsBuffer.hpp"
#include "Graphics/Texture.hpp"
#include <cassert>

using namespace nima;

GameActorImage::GameActorImage() :
	m_DeformVertexBuffer(nullptr),
	m_IndexOffset(0)
{

}

GameActorImage::~GameActorImage()
{
	delete m_DeformVertexBuffer;
}

ActorNode* GameActorImage::makeInstance(Actor* resetActor)
{
	ActorImage* instanceNode = new GameActorImage();
	instanceNode->copy(this, resetActor);
	return instanceNode;
}

GameActor::GameActor() :
	m_Textures(nullptr),
	m_VertexBuffer(nullptr),
	m_SkinnedVertexBuffer(nullptr),
	m_IndexBuffer(nullptr)
{

}

GameActor::~GameActor()
{
	if(m_Textures != nullptr)
	{
		for(int i = 0; i < m_MaxTextureIndex; i++)
		{
			delete m_Textures[i];
		}
		delete [] m_Textures;
	}
	delete m_VertexBuffer;
	delete m_SkinnedVertexBuffer;
	delete m_IndexBuffer;
}

void GameActor::dispose()
{
	Base::dispose();
	if(m_Textures != nullptr)
	{
		for(int i = 0; i < m_MaxTextureIndex; i++)
		{
			delete m_Textures[i];
		}
		delete [] m_Textures;
	}
	delete m_VertexBuffer;
	delete m_SkinnedVertexBuffer;
	delete m_IndexBuffer;

	m_Textures = nullptr;
	m_VertexBuffer = nullptr;
	m_SkinnedVertexBuffer = nullptr;
	m_IndexBuffer = nullptr;
}

ActorImage* GameActor::makeImageNode()
{
	return new GameActorImage();
}

void GameActor::initialize(Renderer2D* renderer)
{
	// We are initializing the shared actor data. Make two vertex buffers for the two different data strides we support (12 skinned) (4 unskinned).
	assert(m_VertexBuffer == nullptr);

	std::vector<float> vertexData;
	std::vector<float> skinnedVertexData;
	std::vector<unsigned short> indexData;

	for(int i = 0; i < m_ImageNodeCount; i++)
	{
		GameActorImage* actorImage = reinterpret_cast<GameActorImage*>(m_ImageNodes[i]);
		// N.B. Even vertex deformed buffers get full stride. This wastes a little bit of data as each vertex deformed
		// mesh will also have their original positions stored on the GPU, but this saves quite a bit of extra branching.

		std::vector<float>& currentVertexData = actorImage->connectedBoneCount() > 0 ? skinnedVertexData : vertexData;
		
		// Calculate the offset in our contiguous vertex buffer.
		unsigned short firstVertexIndex = (unsigned short)(currentVertexData.size()/actorImage->vertexStride());
		float* vertices = actorImage->vertices();
		int size = actorImage->vertexCount() * actorImage->vertexStride();
		for(int j = 0; j < size; j++)
		{
			currentVertexData.push_back(vertices[j]);
		}

		// N.B. There's an implication here that each mesh cannot have more than 65,535 vertices.
		actorImage->m_IndexOffset = indexData.size();
		unsigned short* tris = actorImage->triangles();
		int indexCount = actorImage->triangleCount() * 3;
		for(int j = 0; j < indexCount; j++)
		{
			indexData.push_back(tris[j]+firstVertexIndex);
		}
	}

	// The buffers allocated here are all static as they do not change at runtime.
	if(vertexData.size() > 0)
	{
		m_VertexBuffer = renderer->makeVertexBuffer();
		m_VertexBuffer->setData(vertexData.data(), sizeof(float) * vertexData.size(), BufferHint::Static);
	}
	if(skinnedVertexData.size() > 0)
	{
		m_SkinnedVertexBuffer = renderer->makeVertexBuffer();
		m_SkinnedVertexBuffer->setData(skinnedVertexData.data(), sizeof(float) * skinnedVertexData.size(), BufferHint::Static);
	}
	if(indexData.size() > 0)
	{
		m_IndexBuffer = renderer->makeIndexBuffer();
		m_IndexBuffer->setData(indexData.data(), sizeof(unsigned short) * indexData.size(), BufferHint::Static);
	}
}

GameActorInstance* GameActor::makeInstance()
{
	GameActorInstance* instance = new GameActorInstance();
	instance->copy(*this);
	return instance;
}

GameActorInstance::GameActorInstance()
{

}

GameActorInstance::~GameActorInstance()
{

}

void GameActorInstance::initialize(Renderer2D* renderer)
{
	// When we initialize a character instance we go and generate the per instance graphical data necessary.
	// In this case, each image that vertex deforms via animation will get its own buffer...
	// We could potentially make this one contiguous one too, but the assumption would be that the entire buffer
	// would be changing each frame.
	for(int i = 0; i < m_ImageNodeCount; i++)
	{
		GameActorImage* actorImage = reinterpret_cast<GameActorImage*>(m_ImageNodes[i]);
		// If we have vertex deformation, we get a custom vertex buffer.
		if(actorImage->doesAnimationVertexDeform())
		{
			actorImage->m_DeformVertexBuffer = renderer->makeVertexBuffer();
			actorImage->m_DeformVertexBuffer->setData(actorImage->animationDeformedVertices(), sizeof(float) * actorImage->vertexCount() * 2 /*2 floats per deform data, the x and y translation value*/, BufferHint::Dynamic);
		}
	}
}