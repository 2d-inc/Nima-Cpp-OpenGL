#ifndef _ARCHERCONTROLLER_HPP_
#define _ARCHERCONTROLLER_HPP_

#include "GameActor.hpp"
#include <nima/Animation/ActorAnimation.hpp>

class ArcherController : public nima::GameActorController
{
	private:
		nima::ActorAnimation* m_Idle;
		nima::ActorAnimation* m_Aim;
		//nima::ActorAnimation* m_Walk;
		nima::ActorAnimationInstance* m_Walk;
		nima::ActorAnimation* m_Run;
		nima::ActorAnimation* m_WalkToIdle;
		nima::CustomFloatProperty* m_GroundSpeedProperty;
		float m_AimAnimationTime;
		float m_IdleTime;
		float m_WalkToIdleTime;
		//float m_WalkTime;
		float m_RunTime;
		float m_HorizontalSpeed;
		bool m_IsRunning;
		float m_WalkMix;
		float m_RunMix;

		struct AimSlice
		{
			nima::Vec2D point;
			nima::Vec2D dir;
		};
		constexpr static const int AimSliceCount = 60;
		constexpr static const float MixSpeed = 3.5f;

		AimSlice m_AimLookup[AimSliceCount];
		AimSlice m_AimWalkingLookup[AimSliceCount];
		nima::Vec2D m_WorldTarget;

	public:
		ArcherController();
		~ArcherController() override;

		void onAdded(nima::GameActorInstance* actorInstance) override;
		void onRemoved(nima::GameActorInstance* actorInstance) override;
		void advance(nima::GameActorInstance* actorInstance, float seconds) override;

		void setAimTarget(const nima::Vec2D& worldTargetTranslation);

		void moveLeft(bool move);
		void moveRight(bool move);
		void run(bool run);
	private:
		static void onAnimationEvent(const nima::ActorAnimationEvent& event, void* userdata);
};


#endif