#pragma once
#include <cheat-base/cheat/Feature.h>
#include <cheat-base/config/Config.h>

namespace cheat::feature 
{

	class ESP : public Feature
    {
	public:
		enum class DrawMode
		{
			None,
			Rectangle,
			Box
		};

		config::field::ToggleField m_Enabled;
		config::field::EnumField<DrawMode> m_DrawBoxMode;
		config::field::BaseField<bool> m_Fill;
		config::field::BaseField<float> m_FillTransparency;

		config::field::BaseField<bool> m_DrawLine;
		config::field::BaseField<bool> m_DrawDistance;
		config::field::BaseField<bool> m_DrawName;

		config::field::BaseField<float> m_FontSize;
		config::field::ColorField m_FontColor;

		config::field::BaseField<float> m_MinSize;
		config::field::BaseField<float> m_Range;

		static ESP& GetInstance();

		const FeatureGUIInfo& GetGUIInfo() const override;
		void DrawMain() override;

		virtual bool NeedStatusDraw() const override;
		void DrawStatus() override;

	private:
		ESP();
	};
}

