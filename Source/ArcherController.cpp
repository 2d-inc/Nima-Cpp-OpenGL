#include "ArcherController.hpp"

ArcherController::ArcherController()
{

}

ArcherController::~ArcherController()
{

}

void ArcherController::onAdded(nima::GameActorInstance* actorInstance)
{
	m_Aim = actorInstance->animation("Aim2");
	if(m_Aim != nullptr)
	{
		nima::ActorNode* muzzle = actorInstance->node("Muzzle");
		if(muzzle != nullptr)
		{
			for(int i = 0; i < AimSliceCount; i++)
			{
				float position = i / (float)(AimSliceCount-1) * m_Aim->duration();
				m_Aim->apply(position, actorInstance, 1.0f);
				auto worldTransform = muzzle->worldTransform();
				AimSlice& slice = m_AimLookup[i];
				nima::Vec2D::normalize(slice.point, nima::Vec2D(worldTransform[0], worldTransform[1]));
				slice.dir[0] = worldTransform[4];
				slice.dir[4] = worldTransform[5];
			}
		}
	}
	/*var aim = this._AimAnimation = new AnimationInstance(actor.getAnimation("Aim2"));
				if(aim)
				{
					// Find arrow node.
					var arrowNode = null;
					for(var i = 0; i < actor._Nodes.length; i++)
					{
						var node = actor._Nodes[i];

						if(node._Name === "Muzzle")
						{
							arrowNode = node;
							break;
						}
					}
					// Build look up table.
					if(arrowNode)
					{
						for(var i = 0; i < _AimLookup.length; i++)
						{
							var position = i / (_AimLookup.length-1) * aim._Max;
							aim.setTime(position, true);
							aim.apply(actor, 1.0);
							var m = arrowNode.getWorldTransform();
							_AimLookup[i] = [
								vec2.normalize(vec2.create(), vec2.set(vec2.create(), m[0], m[1])),
								vec2.set(vec2.create(), m[4], m[5]),
							];
						}

						// Apply first frame of walk to extract the aim while walking lookup.
						this._WalkAnimation.apply(actor, 1.0);
						for(var i = 0; i < _AimWalkingLookup.length; i++)
						{
							var position = i / (_AimWalkingLookup.length-1) * aim._Max;
							aim.setTime(position, true);
							aim.apply(actor, 1.0);
							var m = arrowNode.getWorldTransform();
							_AimWalkingLookup[i] = [
								vec2.normalize(vec2.create(), vec2.set(vec2.create(), m[0], m[1])),
								vec2.set(vec2.create(), m[4], m[5]),
							];
						}
					}*/
}

void ArcherController::onRemoved(nima::GameActorInstance* actorInstance)
{

}

void ArcherController::advance(nima::GameActorInstance* actorInstance, float seconds)
{
	
	if(m_Aim != nullptr)
	{
		nima::ActorNode* root = actorInstance->root();
		nima::Mat2D inverseToActor;
		nima::Vec2D actorTarget;
		nima::Mat2D::invert(inverseToActor, root->worldTransform());
		nima::Vec2D::transform(actorTarget, m_WorldTarget, inverseToActor);

		// See where the target is relative to the tip of the weapon
		float maxDot = -1.0f;
		int bestIndex = 0;
		AimSlice (&lookup)[AimSliceCount] = m_AimLookup;
		//auto lookup = m_AimLookup;//_This._HorizontalSpeed === 0 ? _AimLookup : _AimWalkingLookup;
		for(int i = 0; i < AimSliceCount; i++)
		{
			AimSlice& aim = lookup[i];
			//var aimDir = vec2.clone(aim[0]);
			//var aimPos = vec2.clone(aim[1]);

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

		float aimTime = bestIndex/(float)(AimSliceCount-1) * m_Aim->duration();
		m_Aim->apply(aimTime, actorInstance, 1.0f);
	}

	/*animationTime += elapsed;
	while(animationTime > animation->duration())
	{
		animationTime -= animation->duration();
	}
	animation->apply(animationTime, actorInstance, 1.0f);*/
		
}

void ArcherController::setAimTarget(const nima::Vec2D& worldTargetTranslation)
{
	m_WorldTarget = worldTargetTranslation;
}
