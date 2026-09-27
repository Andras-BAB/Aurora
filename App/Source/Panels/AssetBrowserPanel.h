#pragma once

#include <Aurora.h>

#include <filesystem>

namespace Aurora {

	class AssetBrowserPanel {
	public:
		AssetBrowserPanel();

		void OnImGuiRender();

	private:
		enum class AssetCategory : uint8_t { Folder, Mesh, Texture, Scene, Other };

		struct Entry {
			std::filesystem::path Path;
			std::string Name;
			bool IsDirectory = false;
			bool IsEmpty = false;
			AssetCategory Category = AssetCategory::Other;
			std::shared_ptr<ITexture2D> Thumbnail;
		};

	private:
		void SetCurrentDir(const std::filesystem::path& directory);
		void RefreshCurrentDirectory();

		void DrawTopBar();
		void DrawContentGrid();
		void DrawEntry(Entry& entry, ImVec2 cellSize);

		static AssetCategory ClassifyExtension(std::filesystem::path extension);
		static ImU32 GetCategoryColor(AssetCategory category);

	private:
		std::filesystem::path m_RootDirectory;
		std::filesystem::path m_CurrentDirectory;
		std::filesystem::path m_PendingNavigation;
		std::filesystem::path m_SelectedPath;

		std::vector<Entry> m_Entries;

		ImGuiTextFilter m_Filter;

		float m_ThumbnailSize = 64.0f;
		static constexpr float s_Padding = 20.0f;
	};

}
