#ifndef _NIMA_GAMEACTOR_HPP_
#define _NIMA_GAMEACTOR_HPP_

#include <nima/Actor.hpp>
#include "Graphics/Renderer2D.hpp"

namespace nima
{
	class GameActor;
	class GameActorInstance;

	class GameActorImage : public ActorImage
	{
		friend class GameActor;
		friend class GameActorInstance;
		private:
			// The deform vertex buffer will contain only the deform translation of the vertices.
			GraphicsBuffer* m_DeformVertexBuffer;
			int m_IndexOffset;

		public:
			GameActorImage();
			~GameActorImage();

			ActorNode* makeInstance(Actor* resetActor);
	};

	class GameActor : public Actor
	{
		typedef Actor Base;
		private:
			Texture** m_Textures;
			GraphicsBuffer* m_VertexBuffer;
			GraphicsBuffer* m_SkinnedVertexBuffer;
			GraphicsBuffer* m_IndexBuffer;

		protected:
			ActorImage* makeImageNode() override;
			void dispose() override;
		public:
			GameActor();
			~GameActor();
			void initialize(Renderer2D* renderer);
			GameActorInstance* makeInstance();
	};

	class GameActorInstance : public Actor
	{
		friend class GameActor;
		private:
			GameActorInstance();
		public:
			~GameActorInstance();
			void initialize(Renderer2D* renderer);
			void render(Renderer2D* renderer);
	};
}

#endif