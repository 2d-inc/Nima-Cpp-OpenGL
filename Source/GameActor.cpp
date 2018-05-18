#include "GameActor.hpp"
#include "Graphics/GraphicsBuffer.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Bitmap/DecodeBitmapException.hpp"
#include "Graphics/Bitmap/UnknownBitmapFormatException.hpp"
#include <nima/Exceptions/MissingFileException.hpp>
#include <cassert>
#include <algorithm>

using namespace nima;

GameActorImage::GameActorImage() :
	m_DeformVertexBuffer(nullptr),
	m_VertexBuffer(nullptr),
	m_IndexOffset(0)
{

}

GameActorImage::~GameActorImage()
{
	delete m_DeformVertexBuffer;
}

void GameActorImage::copy(GameActorImage* node, Actor* resetActor)
{
	Base::copy(node, resetActor);
	m_IndexOffset = node->m_IndexOffset;
	m_VertexBuffer = node->m_VertexBuffer;
}

ActorNode* GameActorImage::makeInstance(Actor* resetActor)
{
	GameActorImage* instanceNode = new GameActorImage();
	instanceNode->copy(this, resetActor);
	return instanceNode;
}

static Color WhiteColor(1.0f, 1.0f, 1.0f, 1.0f);

void GameActorImage::render(GameActorInstance* gameActorInstance, Renderer2D* renderer)
{
	if(textureIndex() < 0 || renderOpacity() <= 0.0f)
	{
		return;
	}
	renderer->setBlendMode(blendMode());

	Texture* texture = gameActorInstance->gameActor()->m_Textures[textureIndex()];
	//GraphicsBuffer* vertexBuffer = gameActorInstance->gameActor()->m_VertexBuffer;
	//GraphicsBuffer* skinnedVertexBuffer = gameActorInstance->gameActor()->m_SkinnedVertexBuffer;
	GraphicsBuffer* indexBuffer = gameActorInstance->gameActor()->m_IndexBuffer;

	if(m_DeformVertexBuffer != nullptr && isVertexDeformDirty())
	{
		m_DeformVertexBuffer->setData(animationDeformedVertices(), sizeof(float) * vertexCount() * 2 /*2 floats per deform data, the x and y translation value*/, BufferHint::Dynamic);
		isVertexDeformDirty(false);
	}

	GraphicsBuffer* seqUVBuffer = gameActorInstance->gameActor()->m_SequenceUVBuffer;
	int uvOffset = 0;
	if(seqUVBuffer != nullptr)
	{
		int frame = sequenceFrame();
		if(frame < 0)
		{
			frame += sequenceFramesCount();
		}
		uvOffset = sequenceFrames()[frame].m_Offset;
	}

	renderer->prep(texture, WhiteColor, renderOpacity(), worldTransform(), m_VertexBuffer, boneInfluenceMatrices(), boneInfluenceMatricesLength(), m_DeformVertexBuffer, seqUVBuffer, uvOffset);
	renderer->draw(indexBuffer, triangleCount()*3);

/* 
	if(connectedBoneCount() > 0)
	{
		if(m_DeformVertexBuffer != nullptr)
		{
			renderer->drawTexturedAndDeformedSkin(worldTransform(), m_DeformVertexBuffer, m_VertexBuffer, indexBuffer, m_IndexOffset, triangleCount()*3, boneInfluenceMatrices(), boneInfluenceMatricesLength(), renderOpacity(), WhiteColor, texture);
		}
		else
		{
			renderer->drawTexturedSkin(worldTransform(), m_VertexBuffer, indexBuffer, m_IndexOffset, triangleCount()*3, boneInfluenceMatrices(), boneInfluenceMatricesLength(), renderOpacity(), WhiteColor, texture);
		}
	}
	else
	{
		if(m_DeformVertexBuffer != nullptr)
		{
			renderer->drawTexturedAndDeformed(worldTransform(), m_DeformVertexBuffer, m_VertexBuffer, indexBuffer, m_IndexOffset, triangleCount()*3, renderOpacity(), WhiteColor, texture);
		}
		else
		{
			renderer->drawTextured(worldTransform(), m_VertexBuffer, indexBuffer, m_IndexOffset, triangleCount()*3, renderOpacity(), WhiteColor, texture);
		}
	}
 */
}

GameActor::GameActor() :
	m_Textures(nullptr),
	m_VertexBuffer(nullptr),
	m_SkinnedVertexBuffer(nullptr),
	m_IndexBuffer(nullptr),
	m_SequenceUVBuffer(nullptr)
{

}

GameActor::~GameActor()
{
	// delete m_VertexBuffer from image nodes with deform.
	for(int i = 0; i < m_ImageNodeCount; i++)
	{
		GameActorImage* actorImage = reinterpret_cast<GameActorImage*>(m_ImageNodes[i]);
		if(actorImage->doesAnimationVertexDeform())
		{
			delete actorImage->m_VertexBuffer;
		}
	}
	if(m_Textures != nullptr)
	{
		for(int i = 0; i <= m_MaxTextureIndex; i++)
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
		for(int i = 0; i <= m_MaxTextureIndex; i++)
		{
			delete m_Textures[i];
		}
		delete [] m_Textures;
	}
	delete m_VertexBuffer;
	delete m_SkinnedVertexBuffer;
	delete m_IndexBuffer;
	delete m_SequenceUVBuffer;

	m_Textures = nullptr;
	m_VertexBuffer = nullptr;
	m_SkinnedVertexBuffer = nullptr;
	m_IndexBuffer = nullptr;
	m_SequenceUVBuffer = nullptr;
}

ActorImage* GameActor::makeImageNode()
{
	return new GameActorImage();
}

void GameActor::initializeGraphics(Renderer2D* renderer)
{
	if(textureCount() != 0)
	{
		m_Textures = new Texture*[m_MaxTextureIndex+1];	
		for(int i = 0; i <= m_MaxTextureIndex; i++)
		{
			std::string atlasFilename;
			if(m_MaxTextureIndex == 0)
			{
				atlasFilename = baseFilename() + std::string(".png");
			}
			else
			{
				atlasFilename = baseFilename() + std::to_string(i) + std::string(".png");
			}

			try
			{
				m_Textures[i] = renderer->makeTexture(atlasFilename, Texture::MipMap | Texture::ClampToEdge);
			}
			catch(const DecodeBitmapException& ex)
			{
				printf("Decode bitmap error: %s\n", ex.message().c_str());
			}
			catch(const UnknownBitmapFormatException& ex)
			{
				printf("Unknown format error: %s\n", ex.message().c_str());	
			}
			catch(const MissingFileException& ex)
			{
				printf("Missing file error: %s %s\n", ex.message().c_str(), ex.filename().c_str());	
			}
		}
	}	

	// We are initializing the shared actor data. Make two vertex buffers for the two different data strides we support (12 skinned) (4 unskinned).
	assert(m_VertexBuffer == nullptr);

	std::vector<float> vertexData;
	std::vector<float> skinnedVertexData;
	std::vector<unsigned short> indexData;
	std::vector<float> uvData;

	for(int i = 0; i < m_RenderNodeCount; i++)
	{
		auto renderNode = m_RenderNodes[i];
		switch(renderNode->type())
		{
			case nima::ComponentType::ActorImage:
			{
				GameActorImage* actorImage = reinterpret_cast<GameActorImage*>(renderNode);

				// When the image has vertex deform we actually get two vertex buffers per image.
				// One that is static with all our base vertex data in there and one with the override positions.
				// TODO: optimize this to remove the positions from the base one.
				if(actorImage->doesAnimationVertexDeform())
				{
					actorImage->m_VertexBuffer = renderer->makeVertexBuffer();
					actorImage->m_VertexBuffer->setData(actorImage->vertices(), sizeof(float) * actorImage->vertexCount() * actorImage->vertexStride(), BufferHint::Static);

					actorImage->m_IndexOffset = indexData.size();
					unsigned short* tris = actorImage->triangles();
					int indexCount = actorImage->triangleCount() * 3;
					for(int j = 0; j < indexCount; j++)
					{
						indexData.push_back(tris[j]);
					}
				}
				else
				{
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

					float* seqUVs = actorImage->sequenceUVs();
					if(seqUVs != nullptr)
					{
						int uvsCount = actorImage->vertexCount() * actorImage->sequenceFramesCount() * 2;
						for(int j = 0; j < uvsCount; j++)
						{
							uvData.push_back(seqUVs[j]);
						}
					}
				}
				break;
			}
			default:
			{
				// This example doesn't handle other types yet as they are forthcoming in Nima.
				break;
			}
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
	if(uvData.size() > 0)
	{
		m_SequenceUVBuffer = renderer->makeVertexBuffer();
		m_SequenceUVBuffer->setData(uvData.data(), sizeof(float)*uvData.size(), BufferHint::Static);
	}

	// Update the vertex buffers being referenced.
	for(int i = 0; i < m_RenderNodeCount; i++)
	{
		auto renderNode = m_RenderNodes[i];
		switch(renderNode->type())
		{
			case nima::ComponentType::ActorImage:
			{
				GameActorImage* actorImage = reinterpret_cast<GameActorImage*>(renderNode);

				if(!actorImage->doesAnimationVertexDeform())
				{
					if(actorImage->connectedBoneCount() > 0)
					{
						actorImage->m_VertexBuffer = m_SkinnedVertexBuffer;
					}
					else
					{
						actorImage->m_VertexBuffer = m_VertexBuffer;	
					}
				}
				break;
			}
			default:
			{
				continue;
			}
		}
	}
}

Actor* GameActor::makeInstance() const
{
	GameActorInstance* instance = new GameActorInstance(this);
	instance->copy(*this);
	return instance;
}

GameActorInstance::GameActorInstance(const GameActor* gameActor) :
	m_GameActor(gameActor)
{

}

GameActorInstance::~GameActorInstance()
{
	for(auto controller : m_Controllers)
	{
		controller->onRemoved(this);
		delete controller;
	}
	m_Controllers.clear();
}

void GameActorInstance::advance(float elapsedSeconds)
{
	for(auto controller : m_Controllers)
	{
		controller->advance(this, elapsedSeconds);
	}
	
	Base::advance(elapsedSeconds);
}

void GameActorInstance::removeController(GameActorController* controller)
{
	auto itr = std::find(m_Controllers.begin(), m_Controllers.end(), controller);
	if(itr != m_Controllers.end())
	{
		(*itr)->onRemoved(this);
		delete *itr;
		m_Controllers.erase(itr);
	}
}

void GameActorInstance::initializeGraphics(Renderer2D* renderer)
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

const GameActor* GameActorInstance::gameActor()
{
	return m_GameActor;
}

void GameActorInstance::updateVertexDeform(ActorImage* image)
{
	GameActorImage* actorImage = reinterpret_cast<GameActorImage*>(image);
	actorImage->m_DeformVertexBuffer->setData(actorImage->animationDeformedVertices(), sizeof(float) * actorImage->vertexCount() * 2, BufferHint::Dynamic);	
}

void GameActorInstance::render(Renderer2D* renderer)
{
	for(int i = 0; i < m_RenderNodeCount; i++)
	{
		ActorRenderNode* renderNode = m_RenderNodes[i];
		switch(renderNode->type())
		{
			case ComponentType::ActorImage:
			{
				GameActorImage* actorImage = reinterpret_cast<GameActorImage*>(renderNode);
				if(actorImage != nullptr)
				{
					actorImage->render(this, renderer);
				}
				break;
			}
			default:
				// No other types supported in this example yet.
				break;
		}
	}
}