#pragma once
#include <cheat-base/cheat/Feature.h>
#include <cheat-base/config/Config.h>

namespace cheat::feature 
{

	class AutoOreDestroy : public Feature
    {
	public:
		config::field::ToggleField m_Enabled;
		config::field::BaseField<float> m_Range;

		static AutoOreDestroy& GetInstance();

		const FeatureGUIInfo& GetGUIInfo() const override;
		void DrawMain() override;

		virtual bool NeedStatusDraw() const override;
		void DrawStatus() override;
	
	private:
		AutoOreDestroy();
	};
}

