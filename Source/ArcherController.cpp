#include "ArcherController.hpp"
#include <nima/ActorEvent.hpp>
#include <cmath>

ArcherController::ArcherController() : 
	m_Idle(nullptr),
	m_Aim(nullptr),
	m_Walk(nullptr),
	m_Run(nullptr),
	m_WalkToIdle(nullptr),
	m_AimAnimationTime(0.0f),
	m_IdleTime(0.0f),
	m_WalkToIdleTime(0.0f),
	//m_WalkTime(0.0f),
	m_RunTime(0.0f),
	m_HorizontalSpeed(0.0f),
	m_IsRunning(false),
	m_WalkMix(0.0f),
	m_RunMix(0.0f)
{

}

ArcherController::~ArcherController()
{
	delete m_Walk;
}

void ArcherController::onAnimationEvent(const nima::ActorAnimationEvent& event, void* userdata)
{
	printf("GOT EVENT %s\n", event.actorEvent->name().c_str());
}

void ArcherController::onAdded(nima::GameActorInstance* actorInstance)
{
	actorInstance->eventCallback(ArcherController::onAnimationEvent, this);
	m_Idle = actorInstance->animation("Idle");
	m_Aim = actorInstance->animation("Aim2");
	m_Walk = actorInstance->animationInstance("Walk");
	m_Run = actorInstance->animation("Run");
	m_WalkToIdle = actorInstance->animation("WalkToIdle");

	if(m_Aim != nullptr)
	{
		nima::ActorNode* muzzle = actorInstance->component<nima::ActorNode*>("Muzzle");
		if(muzzle != nullptr)
		{
			for(int i = 0; i < AimSliceCount; i++)
			{
				float position = i / (float)(AimSliceCount-1) * m_Aim->duration();
				m_Aim->apply(position, actorInstance, 1.0f);
				auto worldTransform = muzzle->worldTransform();
				AimSlice& slice = m_AimLookup[i];

				// Extract forward vector and position.
				nima::Vec2D::normalize(slice.dir, nima::Vec2D(worldTransform[0], worldTransform[1]));
				slice.point[0] = worldTransform[4];
				slice.point[1] = worldTransform[5];
			}
			if(m_Walk != nullptr)
			{
				// Apply first frame of walk to extract the aim while walking lookup.
				m_Walk->time(0.0f);
				m_Walk->apply(1.0);
				for(int i = 0; i < AimSliceCount; i++)
				{
					float position = i / (float)(AimSliceCount-1) * m_Aim->duration();
					m_Aim->apply(position, actorInstance, 1.0f);
					auto worldTransform = muzzle->worldTransform();
					AimSlice& slice = m_AimWalkingLookup[i];

					// Extract forward vector and position.
					nima::Vec2D::normalize(slice.dir, nima::Vec2D(worldTransform[0], worldTransform[1]));
					slice.point[0] = worldTransform[4];
					slice.point[1] = worldTransform[5];
				}
			}
		}
	}
}

void ArcherController::onRemoved(nima::GameActorInstance* actorInstance)
{

}

void ArcherController::moveLeft(bool move)
{
	if(move)
	{
		m_HorizontalSpeed = -1.0;
	}
	else if(m_HorizontalSpeed == -1.0f)
	{
		m_HorizontalSpeed = 0.0f;
	}
}

void ArcherController::moveRight(bool move)
{
	if(move)
	{
		m_HorizontalSpeed = 1.0;
	}
	else if(m_HorizontalSpeed == 1.0f)
	{
		m_HorizontalSpeed = 0.0f;
	}
}

void ArcherController::run(bool run)
{
	m_IsRunning = run;
}

void ArcherController::advance(nima::GameActorInstance* actorInstance, float elapsedSeconds)
{
	nima::ActorNode* root = actorInstance->root();

    float scaleX = 1.0f;
    if(m_WorldTarget[0] < root->x())
    {
        scaleX = -1.0;
    }
    root->scaleX(scaleX);

	if(m_Idle != nullptr)
	{
		m_IdleTime = std::fmod(m_IdleTime + elapsedSeconds, m_Idle->duration());
		m_Idle->apply(m_IdleTime, actorInstance, 1.0f);
	}
	
	if(m_HorizontalSpeed != 0.0f)
	{
		if(m_IsRunning)
		{
			if(m_WalkMix > 0.0f)
			{
				m_WalkMix = std::max(0.0f, m_WalkMix - elapsedSeconds * MixSpeed);
			}
			if(m_RunMix < 1.0f)
			{
				m_RunMix = std::min(1.0f, m_RunMix + elapsedSeconds * MixSpeed);
			}
		}
		else
		{
			if(m_WalkMix < 1.0f)
			{
				m_WalkMix = std::min(1.0f, m_WalkMix + elapsedSeconds * MixSpeed);
			}
			if(m_RunMix > 0.0f)
			{
				m_RunMix = std::max(0.0f, m_RunMix - elapsedSeconds * MixSpeed);
			}
		}

		m_WalkToIdleTime = 0.0f;
	}
	else
	{
		if(m_WalkMix > 0.0f)
		{
			m_WalkMix = std::max(0.0f, m_WalkMix - elapsedSeconds * MixSpeed);
		}
		if(m_RunMix > 0.0f)
		{
			m_RunMix = std::max(0.0f, m_RunMix - elapsedSeconds * MixSpeed);
		}
	}

	float moveSpeed = m_IsRunning ? 1100.0f : 600.0f;
	root->x(root->x() + m_HorizontalSpeed * elapsedSeconds * moveSpeed);
	if(m_Walk != nullptr && m_Run != nullptr)
	{
		if(m_HorizontalSpeed == 0.0f && m_WalkMix == 0.0f && m_RunMix == 0.0f)
		{
			m_Walk->time(0.0f);
			//m_WalkTime = 0.0f;
			m_RunTime = 0.0f;
		}
		else
		{
			//m_WalkTime = m_WalkTime + elapsedSeconds * 0.9f * (m_HorizontalSpeed > 0 ? 1.0f : -1.0f) * scaleX;
			// Sync up the run and walk times.
			//m_WalkTime = std::fmod(m_WalkTime, m_Walk->duration());
			//if(m_WalkTime < 0.0f)
			//{
		//		m_WalkTime += m_Walk->duration();
		//	}
			m_Walk->advance(elapsedSeconds * 0.9f * (m_HorizontalSpeed > 0 ? 1.0f : -1.0f) * scaleX);
			m_RunTime = (m_Walk->time() - m_Walk->min()) / m_Walk->max() * m_Run->duration();
		}

		if(m_WalkMix != 0.0f)
		{
			m_Walk->apply(m_WalkMix);
		}
		if(m_RunMix != 0.0f)
		{
			m_Run->apply(m_RunTime, actorInstance, m_RunMix);
		}


		if(m_WalkToIdle != nullptr && m_HorizontalSpeed == 0.0f && m_WalkToIdleTime < m_WalkToIdle->duration())
		{
			m_WalkToIdleTime += elapsedSeconds;
			m_WalkToIdle->apply(m_WalkToIdleTime, actorInstance, std::min(1.0f, m_WalkToIdleTime/m_WalkToIdle->duration()));
			//m_RunMix = m_WalkMix = 0.0;
		}
	}

	if(m_Aim != nullptr)
	{
		nima::Mat2D inverseToActor;
		nima::Vec2D actorTarget;
		nima::Mat2D::invert(inverseToActor, root->worldTransform());
		nima::Vec2D::transform(actorTarget, m_WorldTarget, inverseToActor);
        

		// See where the target is relative to the tip of the weapon
		float maxDot = -1.0f;
		int bestIndex = 0;
		AimSlice (&lookup)[AimSliceCount] = m_HorizontalSpeed == 0.0f ? m_AimLookup : m_AimWalkingLookup;
		
		for(int i = 0; i < AimSliceCount; i++)
		{
			AimSlice& aim = lookup[i];

			nima::Vec2D targetDir;
			nima::Vec2D::subtract(targetDir, actorTarget, aim.point);
			nima::Vec2D::normalize(targetDir, targetDir);
			float d = nima::Vec2D::dot(targetDir, aim.dir);
			if(d > maxDot)
			{
				maxDot = d;
				bestIndex = i;
			}
		}

		float targetAimTime = bestIndex/(float)(AimSliceCount-1) * m_Aim->duration();

		m_AimAnimationTime += (targetAimTime-m_AimAnimationTime) * std::min(1.0f, elapsedSeconds*10.0f);

		m_Aim->apply(m_AimAnimationTime, actorInstance, 1.0f);
	}
		
}

void ArcherController::setAimTarget(const nima::Vec2D& worldTargetTranslation)
{
	m_WorldTarget = worldTargetTranslation;
}
