#include "pch-il2cpp.h"
#include "ESP.h"

#include <helpers.h>
#include <algorithm>

#include <cheat/events.h>

namespace cheat::feature 
{
	ESP::ESP() : Feature(),
		NF(m_Enabled, "ESP", "ESP", false),

        NF(m_DrawBoxMode, "Draw mode", "ESP", DrawMode::Box),
        NF(m_Fill, "Fill box/rentangle", "ESP", false),
        NF(m_FillTransparency, "Fill transparency", "ESP", 0.5f),

		NF(m_DrawLine, "Draw line", "ESP", false),
        NF(m_DrawDistance, "Draw distance", "ESP", false),
        NF(m_DrawName, "Draw name", "ESP", false),

        NF(m_FontSize, "Font size", "ESP", 12.0f),
        NF(m_FontColor, "Font color", "ESP", ImColor(255, 255, 255)),

        NF(m_MinSize, "Min in world size", "ESP", 0.5f),
		NF(m_Range, "Range", "ESP", 100.0f)
    { 
	}

    const FeatureGUIInfo& ESP::GetGUIInfo() const
    {
        static const FeatureGUIInfo info { "-General", "ESP", true };
        return info;
    }

    void ESP::DrawMain()
    {
		ConfigWidget("ESP Enabled", m_Enabled, "Show filtered object through obstacles.");
        ConfigWidget(m_Range, 1.0f, 1.0f, 200.0f);
        
        ConfigWidget(m_DrawBoxMode, "Select the mode of box drawing.");
		ConfigWidget(m_Fill);
		ConfigWidget(m_FillTransparency, 0.01f, 0.0f, 1.0f, "Transparency of filled part.");

        ImGui::Spacing();
        ConfigWidget(m_DrawLine,     "Show line from character to object on screen.");
        ConfigWidget(m_DrawName,     "Draw name about object.");
        ConfigWidget(m_DrawDistance, "Draw distance about object.");

        ImGui::Spacing();
        ConfigWidget(m_FontSize, 0.05f, 1.0f, 100.0f, "Font size of name or distance.");
        ConfigWidget(m_FontColor, "Color of name or distance text font.");

        ConfigWidget(m_MinSize, 0.05f, 0.1f, 200.0f, "Minimal object size in world.\n"
            "Some entities have not bounds or bounds is too small, this parameter help set minimal size of this type object.");
    }

    bool ESP::NeedStatusDraw() const
	{
        return m_Enabled;
    }

    void ESP::DrawStatus() 
    { 
        ImGui::Text("ESP [%.01fm|%s%s%s]", m_Range.value(), 
            m_DrawBoxMode ? "O" : "", 
            m_Fill ? "F" : "", 
            m_DrawLine ? "L" : "");
    }

    ESP& ESP::GetInstance()
    {
        static ESP instance;
        return instance;
    }
}

