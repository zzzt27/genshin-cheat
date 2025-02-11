#include "pch-il2cpp.h"
#include "ESP.h"

#include <helpers.h>
#include <algorithm>

#include <cheat/events.h>
#include <cheat/game/EntityManager.h>
#include "ESPRender.h"
#include <cheat/game/filters.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

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
		NF(m_ApplyGlobalFontColor, "Apply global font colors", "ESP", false),

        NF(m_MinSize, "Min in world size", "ESP", 0.5f),
		NF(m_Range, "Range", "ESP", 100.0f),
		m_Search({})
    {
		cheat::events::KeyUpEvent += MY_METHOD_HANDLER(ESP::OnKeyUp);
		InstallFilters();
	}


    const FeatureGUIInfo& ESP::GetGUIInfo() const
    {
        static const FeatureGUIInfo info { "", "ESP", false };
        return info;
    }

    void ESP::DrawMain()
    {
		BeginGroupPanel("General", ImVec2(-1, 0));

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
		ConfigWidget(m_ApplyGlobalFontColor, "Override all color settings with above font color setting.\n"
			"Turn off to revert to custom settings.");

        ConfigWidget(m_MinSize, 0.05f, 0.1f, 200.0f, "Minimal object size in world.\n"
            "Some entities have not bounds or bounds is too small, this parameter help set minimal size of this type object.");
		
		EndGroupPanel();

		ImGui::Text("How to use item filters:\n\tLeft Mouse Button (LMB) - toggle visibility.\n\tRMB - change color.");
		ImGui::InputText("Search filters", &m_Search);

		for (auto& [section, filters] : m_Sections)
		{
			ImGui::PushID(section.c_str());
			DrawSection(section, filters);
			ImGui::PopID();
		}
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

	void ESP::AddFilter(const std::string& section, const std::string& name, game::IEntityFilter* filter)
	{
		if (m_Sections.count(section) == 0)
			m_Sections[section] = {};

		auto& filters = m_Sections[section];
		filters.push_back({ new config::field::ESPItemField(name, name, section), filter });
		
		auto& last = filters.back();
		config::AddField(*last.first);
	}

	void ESP::DrawSection(const std::string& section, const Filters& filters)
	{
		std::vector<const FilterInfo*> validFilters;
		
		for (auto& info : filters)
		{
			//m0nkrel : We making a string copies and lowercase them to avoid case sensitive search
			//Yes, it's shitcode and maybe it break something, but it works.
			std::string name = info.first->value().m_Name;
			std::transform(name.begin(), name.end(), name.begin(), ::tolower);
			std::string search = m_Search;
			std::transform(search.begin(), search.end(), search.begin(), ::tolower);
			if (name.find(search) != std::string::npos) 
				validFilters.push_back(&info);
		}

		if (validFilters.size() == 0)
			return;

		BeginGroupPanel(section.c_str(), ImVec2(-1, 0));

		for (auto& info: validFilters)
		{
			ImGui::PushID(info->first);
			DrawFilterField(*info->first);
			ImGui::PopID();
		}

		ImGui::Spacing();

		if (ImGui::TreeNode(this, "Hotkeys"))
		{
			for (auto& info : validFilters)
			{
				auto& field = info->first;
				ImGui::PushID(field);

				auto& hotkey = field->valuePtr()->m_EnabledHotkey;
				if (InputHotkey(field->GetName().c_str(), &hotkey, true))
					field->Check();

				ImGui::PopID();
			}

			ImGui::TreePop();
		}

		EndGroupPanel();
	}

	std::string Unsplit(const std::string& value)
	{
		std::stringstream in(value);
		std::stringstream out;
		std::string word;
		while (in >> word)
			out << word;
		return out.str();
	}

	void FilterItemSelector(const char* label, ImTextureID image, config::field::ESPItemField& field, const ImVec2& size = ImVec2(200, 0), float icon_size = 30);

	void ESP::DrawFilterField(config::field::ESPItemField& field)
	{
		auto imageInfo = ImageLoader::GetImage(Unsplit(field.value().m_Name));
		FilterItemSelector(field.value().m_Name.c_str(), imageInfo ? imageInfo->textureID : nullptr, field);
	}

	void ESP::DrawExternal()
	{
		auto& esp = ESP::GetInstance();
		if (!esp.m_Enabled)
			return;

		esp::render::PrepareFrame();

		auto& entityManager = game::EntityManager::instance();

		for (auto& entity : entityManager.entities())
		{
			if (entityManager.avatar()->distance(entity) > esp.m_Range)
				continue;

			for (auto& [section, filters] : m_Sections)
			{
				for (auto& [field, filter] : filters)
				{
					auto& entry = *field->valuePtr();
					if (!entry.m_Enabled || !m_FilterExecutor.ApplyFilter(entity, filter))
						continue;

					ImColor entityColor = entry.m_Color;
					esp::render::DrawEntity(entry.m_Name, entity, entityColor);
					break;
				}
			}
		}
	}

	void ESP::OnKeyUp(short key, bool& cancelled)
	{
		for (auto& [section, filters] : m_Sections)
		{
			for (auto& [field, filter] : filters)
			{
				auto& entry = *field->valuePtr();
				if (entry.m_EnabledHotkey.IsPressed(key))
				{
					entry.m_Enabled = !entry.m_Enabled;
					field->Check();
				}
			}
		}
	}

	void FilterItemSelector(const char* label, ImTextureID image, config::field::ESPItemField& field, const ImVec2& size, float icon_size)
	{

		// Init ImGui
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return;

		ImGuiContext& g = *GImGui;
		ImGuiIO& io = g.IO;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
		const ImVec2 item_size = ImGui::CalcItemSize(size, ImGui::CalcItemWidth(), label_size.y + style.FramePadding.y * 2.0f + 20.0f);

		float region_max_x = ImGui::GetContentRegionMaxAbs().x;
		if (region_max_x - window->DC.CursorPos.x < item_size.x)
			ImGui::Spacing();

		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + item_size);
		const ImRect total_bb(window->DC.CursorPos, { frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Max.y });

		ImGui::ItemSize(total_bb, style.FramePadding.y);
		if (!ImGui::ItemAdd(total_bb, id))
		{
			ImGui::SameLine();
			return;
		}
		const bool hovered = ImGui::ItemHoverable(frame_bb, id);
		if (hovered)
		{
			ImGui::SetHoveredID(id);
			g.MouseCursor = ImGuiMouseCursor_Hand;
		}

		const bool lmb_click = hovered && io.MouseClicked[0];
		if (lmb_click)
		{
			auto& value = field.valuePtr()->m_Enabled;
			value = !value;
			field.Check();
			ImGui::FocusWindow(window);
			memset(io.MouseDown, 0, sizeof(io.MouseDown));
		}

		const bool rmb_click = hovered && io.MouseClicked[ImGuiMouseButton_Right];

		ImGuiWindow* picker_active_window = NULL;
		
		static bool color_changed = false;
		static ImGuiID opened_id = 0;
		if (rmb_click)
		{
			auto& col = field.valuePtr()->m_Color;
			// Store current color and open a picker
			g.ColorPickerRef = ImVec4(col);
			ImGui::OpenPopup("picker");
			ImGui::SetNextWindowPos(g.LastItemData.Rect.GetBL() + ImVec2(0.0f, style.ItemSpacing.y));
			opened_id = id;
			memset(io.MouseDown, 0, sizeof(io.MouseDown));
		}

		if (ImGui::BeginPopup("picker"))
		{
			picker_active_window = g.CurrentWindow;
			ImGuiColorEditFlags picker_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaBar;
			ImGuiColorEditFlags picker_flags = ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf;
			ImGui::SetNextItemWidth(ImGui::GetFrameHeight() * 12.0f); // Use 256 + bar sizes?
			color_changed |= ImGui::ColorPicker4("##picker", reinterpret_cast<float*>(&field.valuePtr()->m_Color), picker_flags, &g.ColorPickerRef.x);
			ImGui::EndPopup();
		}

		bool popup_closed = id == opened_id && picker_active_window == NULL;
		if (popup_closed)
		{
			opened_id = 0;
			if (color_changed)
			{
				field.Check();
				color_changed = false;
			}
		}

		const ImU32 border_color = ImGui::GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
		const ImRect clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + item_size.x, frame_bb.Min.y + item_size.y); // Not using frame_bb.Max because we have adjusted size
		
		float border_size = 2.0f;
		float border_rounding = 10.0f;

		ImVec2 render_pos = frame_bb.Min + style.FramePadding;
		ImGui::RenderFrame(clip_rect.Min, clip_rect.Max, field.value().m_Color, false, border_rounding);

		if (field.value().m_Enabled)
		{
			float check_mark_size = 13.0f;
			ImVec2 checkStart = { clip_rect.Max.x - check_mark_size - border_size, clip_rect.Min.y };
			ImGui::RenderFrame(checkStart - ImVec2(2.0f, 0), checkStart + ImVec2(check_mark_size + border_size, check_mark_size + border_size + 2.0f),
				ImGui::GetColorU32(ImGuiCol_FrameBg), false, border_rounding);
			ImGui::RenderCheckMark(window->DrawList, checkStart + ImVec2(0, 1.0f), 0xFFFFFFFF, check_mark_size - 1.0f);
		}

		window->DrawList->AddRect(clip_rect.Min, clip_rect.Max, border_color, border_rounding - 1.0f, 0, border_size);

		float y_center = frame_bb.Min.y + (frame_bb.Max.y - frame_bb.Min.y) / 2;

		ImVec2 image_start(frame_bb.Min.x + style.FramePadding.x + 5.0f, y_center - icon_size / 2);
		ImVec2 image_end(image_start.x + icon_size, image_start.y + icon_size);
		if (image != nullptr)
			window->DrawList->AddImageRounded(image, image_start, image_start + ImVec2(icon_size, icon_size), { 0.0f, 0.0f }, { 1.0f, 1.0f },
				ImColor(1.0f, 1.0f, 1.0f), 0.3f);

		bool pushed = ImGui::PushStyleColorWithContrast(field.value().m_Color, ImGuiCol_Text, ImColor(0,0,0), 2.0f);

		ImVec2 text_end(frame_bb.Max.x - style.FramePadding.x - border_size, y_center + label_size.y / 2);
		ImVec2 text_start(ImMax(image_end.x + style.FramePadding.x, text_end.x - label_size.x), y_center - label_size.y / 2);
		ImGui::RenderTextClipped(text_start, text_end, label, NULL, NULL, {0, 0}, &clip_rect);

		if (pushed)
			ImGui::PopStyleColor();

		ImGui::SameLine();

		return;
	}

	std::string SplitWords(const std::string& value)
	{
		std::stringstream outStream;
		std::stringstream inStream(value);

		char ch;
		inStream >> ch;
		outStream << ch;
		while (inStream >> ch)
		{
			if (isupper(ch))
				outStream << " ";
			outStream << ch;
		}
		return outStream.str();
	}

	std::string MakeCapital(std::string value)
	{
		if (islower(value[0]))
			value[0] = toupper(value[0]);
		return value;
	}

#define ADD_FILTER_FIELD(section, name) AddFilter(MakeCapital(#section), SplitWords(#name), &game::filters::##section##::##name##)
	void ESP::InstallFilters()
	{
		ADD_FILTER_FIELD(collection, Book);
		ADD_FILTER_FIELD(collection, Viewpoint);
		ADD_FILTER_FIELD(collection, RadiantSpincrystal);
		ADD_FILTER_FIELD(collection, BookPage);
		ADD_FILTER_FIELD(collection, QuestInteract);

		ADD_FILTER_FIELD(chest, CommonChest);
		ADD_FILTER_FIELD(chest, ExquisiteChest);
		ADD_FILTER_FIELD(chest, PreciousChest);
		ADD_FILTER_FIELD(chest, LuxuriousChest);
		ADD_FILTER_FIELD(chest, RemarkableChest);

		ADD_FILTER_FIELD(featured, Anemoculus);
		ADD_FILTER_FIELD(featured, CrimsonAgate);
		ADD_FILTER_FIELD(featured, Electroculus);
		ADD_FILTER_FIELD(featured, Electrogranum);
		ADD_FILTER_FIELD(featured, FishingPoint);
		ADD_FILTER_FIELD(featured, Geoculus);
		ADD_FILTER_FIELD(featured, KeySigil);
		ADD_FILTER_FIELD(featured, Lumenspar);
		ADD_FILTER_FIELD(featured, ShrineOfDepth);
		ADD_FILTER_FIELD(featured, TimeTrialChallenge);

		ADD_FILTER_FIELD(guide, CampfireTorch);
		ADD_FILTER_FIELD(guide, MysteriousCarvings);
		ADD_FILTER_FIELD(guide, PhaseGate);
		ADD_FILTER_FIELD(guide, Pot);
		ADD_FILTER_FIELD(guide, RuinBrazier);
		ADD_FILTER_FIELD(guide, Stormstone);

		ADD_FILTER_FIELD(living, BirdEgg);
		ADD_FILTER_FIELD(living, ButterflyWings);
		ADD_FILTER_FIELD(living, Crab);
		ADD_FILTER_FIELD(living, CrystalCore);
		ADD_FILTER_FIELD(living, Fish);
		ADD_FILTER_FIELD(living, Frog);
		ADD_FILTER_FIELD(living, LizardTail);
		ADD_FILTER_FIELD(living, LuminescentSpine);
		ADD_FILTER_FIELD(living, Onikabuto);
		ADD_FILTER_FIELD(living, Starconch);
		ADD_FILTER_FIELD(living, UnagiMeat);

		ADD_FILTER_FIELD(mineral, AmethystLump);
		ADD_FILTER_FIELD(mineral, ArchaicStone);
		ADD_FILTER_FIELD(mineral, CorLapis);
		ADD_FILTER_FIELD(mineral, CrystalChunk);
		ADD_FILTER_FIELD(mineral, CrystalMarrow);
		ADD_FILTER_FIELD(mineral, ElectroCrystal);
		ADD_FILTER_FIELD(mineral, IronChunk);
		ADD_FILTER_FIELD(mineral, NoctilucousJade);
		ADD_FILTER_FIELD(mineral, MagicalCrystalChunk);
		ADD_FILTER_FIELD(mineral, StarSilver);
		ADD_FILTER_FIELD(mineral, WhiteIronChunk);

		ADD_FILTER_FIELD(monster, AbyssMage);
		ADD_FILTER_FIELD(monster, FatuiAgent);
		ADD_FILTER_FIELD(monster, FatuiCicinMage);
		ADD_FILTER_FIELD(monster, FatuiMirrorMaiden);
		ADD_FILTER_FIELD(monster, FatuiSkirmisher);
		ADD_FILTER_FIELD(monster, FloatingFungus);
		ADD_FILTER_FIELD(monster, Geovishap);
		ADD_FILTER_FIELD(monster, GeovishapHatchling);
		ADD_FILTER_FIELD(monster, Hilichurl);
		ADD_FILTER_FIELD(monster, Mitachurl);
		ADD_FILTER_FIELD(monster, Nobushi);
		ADD_FILTER_FIELD(monster, RuinGuard);
		ADD_FILTER_FIELD(monster, RuinHunter);
		ADD_FILTER_FIELD(monster, RuinSentinel);
		ADD_FILTER_FIELD(monster, Samachurl);
		ADD_FILTER_FIELD(monster, ShadowyHusk);
		ADD_FILTER_FIELD(monster, Slime);
		ADD_FILTER_FIELD(monster, Specter);
		ADD_FILTER_FIELD(monster, TreasureHoarder);
		ADD_FILTER_FIELD(monster, UnusualHilichurl);
		ADD_FILTER_FIELD(monster, Whopperflower);
		ADD_FILTER_FIELD(monster, WolvesOfTheRift);

		ADD_FILTER_FIELD(plant, AmakumoFruit);
		ADD_FILTER_FIELD(plant, Apple);
		ADD_FILTER_FIELD(plant, BambooShoot);
		ADD_FILTER_FIELD(plant, Berry);
		ADD_FILTER_FIELD(plant, CallaLily);
		ADD_FILTER_FIELD(plant, Carrot);
		ADD_FILTER_FIELD(plant, Cecilia);
		ADD_FILTER_FIELD(plant, DandelionSeed);
		ADD_FILTER_FIELD(plant, Dendrobium);
		ADD_FILTER_FIELD(plant, FlamingFlowerStamen);
		ADD_FILTER_FIELD(plant, FluorescentFungus);
		ADD_FILTER_FIELD(plant, GlazeLily);
		ADD_FILTER_FIELD(plant, Horsetail);
		ADD_FILTER_FIELD(plant, JueyunChili);
		ADD_FILTER_FIELD(plant, LavenderMelon);
		ADD_FILTER_FIELD(plant, LotusHead);
		ADD_FILTER_FIELD(plant, Matsutake);
		ADD_FILTER_FIELD(plant, Mint);
		ADD_FILTER_FIELD(plant, MistFlowerCorolla);
		ADD_FILTER_FIELD(plant, Mushroom);
		ADD_FILTER_FIELD(plant, NakuWeed);
		ADD_FILTER_FIELD(plant, PhilanemoMushroom);
		ADD_FILTER_FIELD(plant, Pinecone);
		ADD_FILTER_FIELD(plant, Qingxin);
		ADD_FILTER_FIELD(plant, Radish);
		ADD_FILTER_FIELD(plant, SakuraBloom);
		ADD_FILTER_FIELD(plant, SangoPearl);
		ADD_FILTER_FIELD(plant, SeaGanoderma);
		ADD_FILTER_FIELD(plant, Seagrass);
		ADD_FILTER_FIELD(plant, SilkFlower);
		ADD_FILTER_FIELD(plant, SmallLampGrass);
		ADD_FILTER_FIELD(plant, Snapdragon);
		ADD_FILTER_FIELD(plant, Sunsettia);
		ADD_FILTER_FIELD(plant, SweetFlower);
		ADD_FILTER_FIELD(plant, Valberry);
		ADD_FILTER_FIELD(plant, Violetgrass);
		ADD_FILTER_FIELD(plant, WindwheelAster);
		ADD_FILTER_FIELD(plant, Wolfhook);

		ADD_FILTER_FIELD(puzzle, AncientRime);
		ADD_FILTER_FIELD(puzzle, BakeDanuki);
		ADD_FILTER_FIELD(puzzle, BloattyFloatty);
		ADD_FILTER_FIELD(puzzle, CubeDevices);
		ADD_FILTER_FIELD(puzzle, EightStoneTablets);
		ADD_FILTER_FIELD(puzzle, ElectricConduction);
		ADD_FILTER_FIELD(puzzle, ElectroSeelie);
		ADD_FILTER_FIELD(puzzle, ElementalMonument);
		ADD_FILTER_FIELD(puzzle, FloatingAnemoSlime);
		ADD_FILTER_FIELD(puzzle, Geogranum);
		ADD_FILTER_FIELD(puzzle, GeoPuzzle);
		ADD_FILTER_FIELD(puzzle, LargeRockPile);
		ADD_FILTER_FIELD(puzzle, LightUpTilePuzzle);
		ADD_FILTER_FIELD(puzzle, LightningStrikeProbe);
		ADD_FILTER_FIELD(puzzle, LuminousSeelie);
		ADD_FILTER_FIELD(puzzle, MistBubble);
		ADD_FILTER_FIELD(puzzle, PirateHelm);
		ADD_FILTER_FIELD(puzzle, PressurePlate);
		ADD_FILTER_FIELD(puzzle, Seelie);
		ADD_FILTER_FIELD(puzzle, SeelieLamp);
		ADD_FILTER_FIELD(puzzle, SmallRockPile);
		ADD_FILTER_FIELD(puzzle, StormBarrier);
		ADD_FILTER_FIELD(puzzle, SwordHilt);
		ADD_FILTER_FIELD(puzzle, TorchPuzzle);
		ADD_FILTER_FIELD(puzzle, UniqueRocks);
		ADD_FILTER_FIELD(puzzle, WarmingSeelie);
		ADD_FILTER_FIELD(puzzle, WindmillMechanism);
	}
#undef ADD_FILTER_FIELD
}

