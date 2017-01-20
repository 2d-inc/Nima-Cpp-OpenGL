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
		typedef ActorImage Base;
		private:
			// The deform vertex buffer will contain only the deform translation of the vertices.
			GraphicsBuffer* m_DeformVertexBuffer;
			int m_IndexOffset;

		public:
			GameActorImage();
			~GameActorImage();
			void copy(GameActorImage* node, Actor* resetActor);

			ActorNode* makeInstance(Actor* resetActor);
			void render(GameActorInstance* gameActorInstance, Renderer2D* renderer);
	};

	class GameActor : public Actor
	{
		friend class GameActorImage;
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
			GameActorInstance(GameActor* gameActor);
			GameActor* m_GameActor;

		public:
			~GameActorInstance();
			void initialize(Renderer2D* renderer);
			void render(Renderer2D* renderer);
			GameActor* gameActor();
		protected:
			void updateVertexDeform(ActorImage* image) override;
	};
}

#endif