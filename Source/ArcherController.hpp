#ifndef _ARCHERCONTROLLER_HPP_
#define _ARCHERCONTROLLER_HPP_

#include "GameActor.hpp"

class ArcherController : public nima::GameActorController
{
	private:
		nima::ActorAnimation* m_Aim;
		float m_AimAnimationTime;
		struct AimSlice
		{
			nima::Vec2D point;
			nima::Vec2D dir;
		};
		static const int AimSliceCount = 40;

		AimSlice m_AimLookup[AimSliceCount];
		nima::Vec2D m_WorldTarget;

	public:
		ArcherController();
		~ArcherController() override;

		void onAdded(nima::GameActorInstance* actorInstance) override;
		void onRemoved(nima::GameActorInstance* actorInstance) override;
		void advance(nima::GameActorInstance* actorInstance, float seconds) override;

		void setAimTarget(const nima::Vec2D& worldTargetTranslation);
};


#endif