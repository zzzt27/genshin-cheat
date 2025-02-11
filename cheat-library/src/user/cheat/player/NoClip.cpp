#include "pch-il2cpp.h"
#include "NoClip.h"

#include <helpers.h>
#include <cheat/events.h>
#include <cheat/game/EntityManager.h>
#include <cheat/game/util.h>
#include <cheat-base/render/renderer.h>

namespace cheat::feature 
{
	static void HumanoidMoveFSM_LateTick_Hook(void* __this, float deltaTime, MethodInfo* method);

    NoClip::NoClip() : Feature(),
        NF(m_Enabled,            "No clip",              "NoClip", false),
        NF(m_Speed,              "Speed",                "NoClip", 5.5f),
        NF(m_CameraRelative,     "Relative to camera",   "NoClip", true),
		NF(m_SneakSpeedEnabled,  "Sneak speed enabled",  "NoClip", false),
		NF(m_SneakSpeedValue,    "Sneak speed",          "NoClip", 1.0f)
    {
		HookManager::install(app::HumanoidMoveFSM_LateTick, HumanoidMoveFSM_LateTick_Hook);

		events::GameUpdateEvent += MY_METHOD_HANDLER(NoClip::OnGameUpdate);
		events::MoveSyncEvent += MY_METHOD_HANDLER(NoClip::OnMoveSync);
    }

    const FeatureGUIInfo& NoClip::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "No clip", "Player", true };
        return info;
    }

    void NoClip::DrawMain()
    {
		ConfigWidget("Enabled", m_Enabled, "Enables no clip.\n" \
            "For move use ('W', 'A', 'S', 'D', 'Space', 'Shift')");

		ConfigWidget(m_Speed, 0.1f, 2.0f, 100.0f, "No clip move speed.\n"\
            "It's not recommended to set value above 5.");
		
        ConfigWidget(m_CameraRelative, "Move performing relative to camera direction. Not avatar facing direction.");
		
		ConfigWidget("", m_SneakSpeedEnabled); ImGui::SameLine();
		ConfigWidget(m_SneakSpeedValue, 0.1f, 2.0f, 100.0f,
			"Override move speed with value.\nPressing LeftCtrl will make you move faster/slower depending on the value you set.");
    }

    bool NoClip::NeedStatusDraw() const
{
        return m_Enabled;
    }

    void NoClip::DrawStatus() 
    {
        ImGui::Text("NoClip [%.01f|%s]", m_Speed.value(), m_CameraRelative ? "CR" : "PR");
    }

    NoClip& NoClip::GetInstance()
    {
        static NoClip instance;
        return instance;
    }

	// No clip update function.
	// We just disabling collision detect and move avatar when no clip moving keys pressed.
	void NoClip::OnGameUpdate()
	{
		static bool isApplied = false;

		auto& manager = game::EntityManager::instance();
		
		if (!m_Enabled && isApplied)
		{
			auto avatarEntity = manager.avatar();
			auto rigidBody = avatarEntity->rigidbody();
			if (rigidBody == nullptr)
				return;

			app::Rigidbody_set_detectCollisions(rigidBody, true, nullptr);
			isApplied = false;
		}

		if (!m_Enabled)
			return;

		isApplied = true;

		auto avatarEntity = manager.avatar();
		auto baseMove = avatarEntity->moveComponent();
		if (baseMove == nullptr)
			return;

		if (renderer::globals::IsInputBlocked)
			return;

		auto rigidBody = avatarEntity->rigidbody();
		if (rigidBody == nullptr)
			return;

		app::Rigidbody_set_detectCollisions(rigidBody, false, nullptr);

		auto cameraEntity = game::Entity(reinterpret_cast<app::BaseEntity*>(manager.mainCamera()));
		auto relativeEntity = m_CameraRelative ? &cameraEntity : avatarEntity;

		float speed = m_Speed.value();
		if (m_SneakSpeedEnabled && Hotkey(VK_LCONTROL).IsPressed())
			speed = m_SneakSpeedValue.value(); 

		app::Vector3 dir = {};
		if (Hotkey('W').IsPressed())
			dir = dir + relativeEntity->forward();

		if (Hotkey('S').IsPressed())
			dir = dir + relativeEntity->back();

		if (Hotkey('D').IsPressed())
			dir = dir + relativeEntity->right();

		if (Hotkey('A').IsPressed())
			dir = dir + relativeEntity->left();

		if (Hotkey(VK_SPACE).IsPressed())
			dir = dir + relativeEntity->up();

		if (Hotkey(ImGuiKey_ModShift).IsPressed())
			dir = dir + relativeEntity->down();

		app::Vector3 prevPos = avatarEntity->relativePosition();
		if (IsVectorZero(prevPos))
			return;

		float deltaTime = app::Time_get_deltaTime(nullptr, nullptr);

		app::Vector3 newPos = prevPos + dir * speed * deltaTime;
		avatarEntity->setRelativePosition(newPos);
	}

	// Fixing player sync packets when no clip
	void NoClip::OnMoveSync(uint32_t entityId, app::MotionInfo* syncInfo)
	{
		static app::Vector3 prevPosition = {};
		static int64_t prevSyncTime = 0;

		if (!m_Enabled)
		{
			prevSyncTime = 0;
			return;
		}

		auto& manager = game::EntityManager::instance();
		if (manager.avatar()->runtimeID() != entityId)
			return;

		auto avatarEntity = manager.avatar();
		if (avatarEntity == nullptr)
			return;

		auto avatarPosition = avatarEntity->absolutePosition();
		auto currentTime = util::GetCurrentTimeMillisec();
		if (prevSyncTime > 0)
		{
			auto posDiff = avatarPosition - prevPosition;
			auto timeDiff = ((float)(currentTime - prevSyncTime)) / 1000;
			auto velocity = posDiff / timeDiff;

			auto speed = GetVectorMagnitude(velocity);
			if (speed > 0.1)
			{
				syncInfo->fields.motionState = (speed < 2) ? app::MotionState__Enum::MotionWalk : app::MotionState__Enum::MotionRun;

				syncInfo->fields.speed_->fields.x = velocity.x;
				syncInfo->fields.speed_->fields.y = velocity.y;
				syncInfo->fields.speed_->fields.z = velocity.z;
			}

			syncInfo->fields.pos_->fields.x = avatarPosition.x;
			syncInfo->fields.pos_->fields.y = avatarPosition.y;
			syncInfo->fields.pos_->fields.z = avatarPosition.z;
		}

		prevPosition = avatarPosition;
		prevSyncTime = currentTime;
	}

	// Disabling standard motion performing.
	// This disabling any animations, climb, jump, swim and so on.
	// But when it disabled, MoveSync sending our last position, so needs to update position in packet.
	static void HumanoidMoveFSM_LateTick_Hook(void* __this, float deltaTime, MethodInfo* method)
	{
		NoClip& noClip = NoClip::GetInstance();
		if (noClip.m_Enabled)
			return;

		callOrigin(HumanoidMoveFSM_LateTick_Hook, __this, deltaTime, method);
	}
}

