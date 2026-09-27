#include "AssetBrowserPanel.h"

#include "Renderer/RenderCommand.h"

#include <imgui.h>

#include <algorithm>
#include <unordered_set>

namespace Aurora {
	namespace {
		bool IsMeshExtension(const std::string& ext) {
			static const std::unordered_set<std::string> extensions = { ".fbx", ".obj", ".gltf", ".glb" };
			return extensions.contains(ext);
		}

		bool IsTextureExtension(const std::string& ext) {
			static const std::unordered_set<std::string> extensions = { ".png", ".jpg", ".jpeg", ".tga", ".bmp" };
			return extensions.contains(ext);
		}

		ImU32 ScaleColorBrightness(ImU32 color, float factor) {
			ImVec4 c = ImGui::ColorConvertU32ToFloat4(color);
			c.x = std::clamp(c.x * factor, 0.0f, 1.0f);
			c.y = std::clamp(c.y * factor, 0.0f, 1.0f);
			c.z = std::clamp(c.z * factor, 0.0f, 1.0f);
			return ImGui::ColorConvertFloat4ToU32(c);
		}

		std::string TruncateToTwoLines(const std::string& text, float wrapWidth) {
			ImFont* font = ImGui::GetFont();
			float fontSize = ImGui::GetFontSize();
			const char* begin = text.c_str();
			const char* end = begin + text.size();

			const char* line1End = font->CalcWordWrapPosition(fontSize, begin, end, wrapWidth);
			if (line1End >= end) return text;

			const char* line2End = font->CalcWordWrapPosition(fontSize, line1End, end, wrapWidth);
			if (line2End >= end) return text;

			const char* ellipsis = "...";
			std::string secondLine(line1End, line2End);
			while (!secondLine.empty()) {
				std::string candidate = secondLine + ellipsis;
				if (ImGui::CalcTextSize(candidate.c_str()).x <= wrapWidth) break;
				secondLine.pop_back();
			}

			return std::string(begin, line1End) + secondLine + ellipsis;
		}

		bool KebabButton(const char* strId) {
			float lineHeight = ImGui::GetFrameHeight();
			ImVec2 size = { lineHeight * 1.4f, lineHeight };

			ImGui::InvisibleButton(strId, size);
			bool clicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);

			ImVec2 min = ImGui::GetItemRectMin();
			ImVec2 max = ImGui::GetItemRectMax();
			ImVec2 center = { (min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f };
			float radius = size.y * 0.06f;
			float spacing = size.y * 0.22f;

			ImU32 color = ImGui::GetColorU32(ImGui::IsItemHovered() ? ImGuiCol_Text : ImGuiCol_TextDisabled);
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			drawList->AddCircleFilled({ center.x - spacing, center.y }, radius, color);
			drawList->AddCircleFilled(center, radius, color);
			drawList->AddCircleFilled({ center.x + spacing, center.y }, radius, color);

			return clicked;
		}

		void DrawFolderIcon(ImDrawList* drawList, ImVec2 min, ImVec2 max, ImU32 color, bool isEmpty) {
			float width = max.x - min.x;
			float centerY = (min.y + max.y) * 0.5f;

			float height = width / 1.2f;
			ImVec2 iconMin = { min.x, centerY - height * 0.5f };
			ImVec2 iconMax = { max.x, centerY + height * 0.5f };

			float rounding = width * 0.1f;
			float tabHeight = height * 0.22f;
			float tabWidth = width * 0.4f;
			float bodyTop = iconMin.y + tabHeight * 0.7f;
			ImVec2 bodyMin = { iconMin.x, bodyTop };

			if (isEmpty) {
				ImU32 outline = ScaleColorBrightness(color, 0.9f);
				drawList->AddRect(iconMin, { iconMin.x + tabWidth, bodyTop }, outline, rounding * 0.6f, ImDrawFlags_RoundCornersTop, 1.5f);
				drawList->AddRect(bodyMin, iconMax, outline, rounding, ImDrawFlags_RoundCornersBottom | ImDrawFlags_RoundCornersTopRight, 1.5f);
			} else {
				ImU32 tabColor = ScaleColorBrightness(color, 0.82f);
				drawList->AddRectFilled(iconMin, { iconMin.x + tabWidth, bodyTop }, tabColor, rounding * 0.6f, ImDrawFlags_RoundCornersTop);
				drawList->AddRectFilled(bodyMin, iconMax, color, rounding, ImDrawFlags_RoundCornersBottom | ImDrawFlags_RoundCornersTopRight);
			}
		}

		void DrawDocumentIcon(ImDrawList* drawList, ImVec2 min, ImVec2 max, ImU32 color, const std::string& extension) {
			ImVec2 size = { max.x - min.x, max.y - min.y };
			float rounding = size.x * 0.06f;

			drawList->AddRectFilled(min, max, color, rounding);

			float foldSize = size.x * 0.22f;
			ImVec2 foldCorner = { max.x, min.y };
			ImVec2 foldA = { max.x - foldSize, min.y };
			ImVec2 foldB = { max.x, min.y + foldSize };
			drawList->AddTriangleFilled(foldCorner, foldA, foldB, IM_COL32(0, 0, 0, 70));

			std::string label = extension;
			for (auto& c : label) c = (char)std::toupper((unsigned char)c);

			ImVec2 textSize = ImGui::CalcTextSize(label.c_str());
			if (textSize.x < size.x) {
				ImVec2 textPos = { min.x + (size.x - textSize.x) * 0.5f, max.y - textSize.y - size.y * 0.12f };
				drawList->AddText(textPos, IM_COL32(255, 255, 255, 235), label.c_str());
			}
		}

		void DrawCubeIcon(ImDrawList* drawList, ImVec2 min, ImVec2 max, ImU32 color) {
			ImVec2 size = { max.x - min.x, max.y - min.y };
			ImVec2 c = { (min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f + size.y * 0.05f };
			float s = std::min(size.x / 1.74f, size.y / 2.0f) * 0.62f;

			ImVec2 top         = { c.x,          c.y - s };
			ImVec2 left        = { c.x - s * 0.87f, c.y - s * 0.5f };
			ImVec2 right       = { c.x + s * 0.87f, c.y - s * 0.5f };
			ImVec2 bottomLeft  = { c.x - s * 0.87f, c.y + s * 0.5f };
			ImVec2 bottomRight = { c.x + s * 0.87f, c.y + s * 0.5f };
			ImVec2 bottom      = { c.x,          c.y + s };

			ImU32 topColor   = ScaleColorBrightness(color, 1.3f);
			ImU32 leftColor  = ScaleColorBrightness(color, 0.75f);
			ImU32 rightColor = ScaleColorBrightness(color, 1.0f);

			drawList->AddQuadFilled(top, right, c, left, topColor);
			drawList->AddQuadFilled(left, c, bottom, bottomLeft, leftColor);
			drawList->AddQuadFilled(c, right, bottomRight, bottom, rightColor);
		}

		void DrawLayersIcon(ImDrawList* drawList, ImVec2 min, ImVec2 max, ImU32 color) {
			ImVec2 size = { max.x - min.x, max.y - min.y };
			float w = size.x * 0.5f, h = size.y * 0.38f;
			float step = size.x * 0.1f;
			ImVec2 base = { (min.x + max.x) * 0.5f - w * 0.5f - step, (min.y + max.y) * 0.5f - h * 0.5f - step };
			float rounding = size.x * 0.05f;

			for (int i = 0; i < 3; i++) {
				ImVec2 p0 = { base.x + step * i, base.y + step * i };
				ImVec2 p1 = { p0.x + w, p0.y + h };
				ImU32 layerColor = ScaleColorBrightness(color, 0.7f + 0.2f * i);
				drawList->AddRectFilled(p0, p1, layerColor, rounding);
				drawList->AddRect(p0, p1, IM_COL32(0, 0, 0, 60), rounding);
			}
		}

		void DrawImageIcon(ImDrawList* drawList, ImVec2 min, ImVec2 max, ImU32 color) {
			ImVec2 size = { max.x - min.x, max.y - min.y };
			float rounding = size.x * 0.08f;

			drawList->AddRectFilled(min, max, ScaleColorBrightness(color, 0.55f), rounding);

			ImVec2 innerMin = { min.x + size.x * 0.12f, min.y + size.y * 0.12f };
			ImVec2 innerMax = { max.x - size.x * 0.12f, max.y - size.y * 0.12f };
			ImVec2 innerSize = { innerMax.x - innerMin.x, innerMax.y - innerMin.y };
			drawList->AddRectFilled(innerMin, innerMax, color, rounding * 0.6f);

			ImVec2 sunCenter = { innerMin.x + innerSize.x * 0.28f, innerMin.y + innerSize.y * 0.3f };
			drawList->AddCircleFilled(sunCenter, innerSize.x * 0.12f, IM_COL32(255, 255, 255, 230));

			ImVec2 mountainTop   = { innerMin.x + innerSize.x * 0.58f, innerMin.y + innerSize.y * 0.38f };
			ImVec2 mountainLeft  = { innerMin.x, innerMax.y };
			ImVec2 mountainRight = { innerMax.x, innerMax.y };
			drawList->AddTriangleFilled(mountainLeft, mountainTop, mountainRight, IM_COL32(255, 255, 255, 210));
		}
	}

	AssetBrowserPanel::AssetBrowserPanel() {
		m_RootDirectory = std::filesystem::current_path() / "Content";
		SetCurrentDir(m_RootDirectory);
	}

	AssetBrowserPanel::AssetCategory AssetBrowserPanel::ClassifyExtension(std::filesystem::path extension) {
		std::string ext = extension.string();
		std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return (char)std::tolower(c); });

		if (IsMeshExtension(ext)) return AssetCategory::Mesh;
		if (IsTextureExtension(ext)) return AssetCategory::Texture;
		if (ext == ".auscene") return AssetCategory::Scene;

		return AssetCategory::Other;
	}

	ImU32 AssetBrowserPanel::GetCategoryColor(AssetCategory category) {
		switch (category) {
			case AssetCategory::Folder:  return IM_COL32(217, 191, 105, 255);
			case AssetCategory::Mesh:    return IM_COL32(214, 122, 60, 255);
			case AssetCategory::Texture: return IM_COL32(87, 163, 175, 255);
			case AssetCategory::Scene:   return IM_COL32(150, 110, 200, 255);
			default:                     return IM_COL32(140, 140, 140, 255);
		}
	}

	void AssetBrowserPanel::SetCurrentDir(const std::filesystem::path& directory) {
		m_CurrentDirectory = directory;
		RefreshCurrentDirectory();
	}

	void AssetBrowserPanel::RefreshCurrentDirectory() {
		m_Entries.clear();

		if (!std::filesystem::exists(m_CurrentDirectory)) return;

		for (auto& item : std::filesystem::directory_iterator(m_CurrentDirectory)) {
			std::string name = item.path().filename().string();
			if (name.empty() || name[0] == '.') continue;

			Entry entry;
			entry.Path = item.path();
			entry.Name = name;
			entry.IsDirectory = item.is_directory();
			entry.Category = entry.IsDirectory ? AssetCategory::Folder : ClassifyExtension(item.path().extension());
			if (entry.IsDirectory) entry.IsEmpty = std::filesystem::is_empty(item.path());

			if (entry.Category == AssetCategory::Texture) {
				entry.Thumbnail = ITexture2D::Create(entry.Path.string());
			}

			m_Entries.push_back(std::move(entry));
		}

		std::sort(m_Entries.begin(), m_Entries.end(), [](const Entry& a, const Entry& b) {
			if (a.IsDirectory != b.IsDirectory) return a.IsDirectory > b.IsDirectory;
			return a.Name < b.Name;
		});
	}

	void AssetBrowserPanel::OnImGuiRender() {
		ImGui::Begin("Asset Browser");

		DrawTopBar();
		ImGui::Separator();
		DrawContentGrid();

		ImGui::End();
	}

	void AssetBrowserPanel::DrawTopBar() {
		if (m_CurrentDirectory != m_RootDirectory) {
			if (ImGui::Button("<-")) {
				SetCurrentDir(m_CurrentDirectory.parent_path());
			}
			ImGui::SameLine();
		}

		std::filesystem::path accumulated = m_RootDirectory;

		ImGui::PushID("##Breadcrumb_Root");
		if (ImGui::Button(m_RootDirectory.filename().string().c_str())) {
			SetCurrentDir(m_RootDirectory);
		}
		ImGui::PopID();

		std::filesystem::path relative = std::filesystem::relative(m_CurrentDirectory, m_RootDirectory);
		if (relative != ".") {
			for (auto& part : relative) {
				accumulated /= part;

				ImGui::SameLine(0.0f, 4.0f);
				ImGui::TextUnformatted("/");
				ImGui::SameLine(0.0f, 4.0f);

				ImGui::PushID(accumulated.string().c_str());
				if (ImGui::Button(part.string().c_str())) {
					SetCurrentDir(accumulated);
				}
				ImGui::PopID();
			}
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(200.0f);
		m_Filter.Draw("##Search");

		ImGui::SameLine();
		if (KebabButton("##ThumbnailSizeButton")) {
			ImGui::OpenPopup("##ThumbnailSizePopup");
		}
		if (ImGui::BeginPopup("##ThumbnailSizePopup")) {
			ImGui::TextUnformatted("Icon size");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(160.0f);
			ImGui::SliderFloat("##IconSize", &m_ThumbnailSize, 48.0f, 192.0f, "%.0f");
			ImGui::EndPopup();
		}
	}

	void AssetBrowserPanel::DrawContentGrid() {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(s_Padding, s_Padding));
		ImGui::BeginChild("##AssetGridRegion", ImVec2(0.0f, 0.0f), ImGuiChildFlags_AlwaysUseWindowPadding);
		ImGui::PopStyleVar();

		float labelHeight = ImGui::GetTextLineHeightWithSpacing() * 2.0f;
		ImVec2 cellSize = { m_ThumbnailSize, m_ThumbnailSize + labelHeight + 4.0f };
		float cellStepX = cellSize.x + s_Padding;
		float cellStepY = cellSize.y + s_Padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = std::max(1, (int)(panelWidth / cellStepX));

		std::vector<Entry*> visible;
		visible.reserve(m_Entries.size());
		for (auto& entry : m_Entries) {
			if (m_Filter.PassFilter(entry.Name.c_str())) visible.push_back(&entry);
		}

		ImVec2 startPos = ImGui::GetCursorScreenPos();

		for (int i = 0; i < (int)visible.size(); i++) {
			int column = i % columnCount;
			int row = i / columnCount;
			ImGui::SetCursorScreenPos({ startPos.x + column * cellStepX, startPos.y + row * cellStepY });

			ImGui::PushID(visible[i]->Name.c_str());
			DrawEntry(*visible[i], cellSize);
			ImGui::PopID();
		}

		int rowCount = ((int)visible.size() + columnCount - 1) / columnCount;
		ImGui::SetCursorScreenPos({ startPos.x, startPos.y + rowCount * cellStepY });
		ImGui::Dummy(ImVec2(1.0f, 1.0f));

		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered()) {
			m_SelectedPath.clear();
		}

		ImGui::EndChild();

		if (!m_PendingNavigation.empty()) {
			SetCurrentDir(m_PendingNavigation);
			m_PendingNavigation.clear();
		}
	}

	void AssetBrowserPanel::DrawEntry(Entry& entry, ImVec2 cellSize) {
		ImVec2 cellPos = ImGui::GetCursorScreenPos();
		ImVec2 iconSize = { m_ThumbnailSize, m_ThumbnailSize };

		bool isSelected = !m_SelectedPath.empty() && entry.Path == m_SelectedPath;
		ImGui::InvisibleButton("##Cell", cellSize);
		bool isHovered = ImGui::IsItemHovered();

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
			m_SelectedPath = entry.Path;
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			if (entry.IsDirectory) {
				m_PendingNavigation = entry.Path;
			} else {
				AU_CORE_INFO("Asset selected: {0}", entry.Path.string());
			}
		}

		if (!entry.IsDirectory && ImGui::BeginDragDropSource()) {
			std::string pathStr = entry.Path.string();
			ImGui::SetDragDropPayload("AURORA_ASSET_PATH", pathStr.c_str(), pathStr.size() + 1);
			ImGui::TextUnformatted(entry.Name.c_str());
			ImGui::EndDragDropSource();
		}

		ImDrawList* drawList = ImGui::GetWindowDrawList();

		ImU32 highlightColor = 0;
		//if (isSelected) highlightColor = ImGui::GetColorU32(ImGuiCol_Header);
		//else if (isHovered) highlightColor = ImGui::GetColorU32(ImGuiCol_Header, 0.6f);

		if (isSelected) highlightColor = ImGui::GetColorU32(IM_COL32(96, 247, 190, 255 * 0.5f));
		else if (isHovered) highlightColor = ImGui::GetColorU32(IM_COL32(96, 247, 190, 255 * 0.5f), 0.6f);

		if (highlightColor) {
			float pad = 4.0f;
			drawList->AddRectFilled({ cellPos.x - pad, cellPos.y - pad }, { cellPos.x + iconSize.x + pad, cellPos.y + iconSize.y + pad }, highlightColor, 6.0f);
		}

		if (entry.Category == AssetCategory::Texture && entry.Thumbnail && entry.Thumbnail->IsLoaded()) {
			auto* textureManager = RenderCommand::GetTextureManager();
			ImTextureID textureID = (ImTextureID)textureManager->GetGPUHandle(entry.Thumbnail->GetHandle()).ptr;
			drawList->AddImage(textureID, cellPos, { cellPos.x + iconSize.x, cellPos.y + iconSize.y });
		} else {
			ImVec2 inset = { iconSize.x * 0.08f, iconSize.y * 0.08f };
			ImVec2 min = { cellPos.x + inset.x, cellPos.y + inset.y };
			ImVec2 max = { cellPos.x + iconSize.x - inset.x, cellPos.y + iconSize.y - inset.y };
			ImU32 color = GetCategoryColor(entry.Category);

			if (entry.IsDirectory) {
				DrawFolderIcon(drawList, min, max, color, entry.IsEmpty);
			} else if (entry.Category == AssetCategory::Mesh) {
				DrawCubeIcon(drawList, min, max, color);
			} else if (entry.Category == AssetCategory::Texture) {
				DrawImageIcon(drawList, min, max, color);
			} else if (entry.Category == AssetCategory::Scene) {
				DrawLayersIcon(drawList, min, max, color);
			} else {
				DrawDocumentIcon(drawList, min, max, color, entry.Path.extension().string());
			}
		}

		ImVec2 labelPos = { cellPos.x, cellPos.y + iconSize.y + 4.0f };
		ImGui::SetCursorScreenPos(labelPos);
		ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + m_ThumbnailSize);
		std::string displayName = TruncateToTwoLines(entry.Name, m_ThumbnailSize);
		ImGui::TextWrapped("%s", displayName.c_str());
		ImGui::PopTextWrapPos();

		if (isHovered && displayName != entry.Name) {
			ImGui::SetTooltip("%s", entry.Name.c_str());
		}
	}

}
