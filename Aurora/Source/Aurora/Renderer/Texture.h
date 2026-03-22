#pragma once

namespace Aurora {

	enum class ImageFormat : uint8_t {
		None = 0,
		R8,
		RGB8,
		RGBA8,
		RGBA32F
	};

	struct TextureSpecification {
		uint32_t Width = 1;
		uint32_t Height = 1;
		ImageFormat Format = ImageFormat::RGBA8;
		bool GenerateMips = true;
	};
	
	struct TextureHandle {
		uint32_t Index = 0xFFFFFFFF;

		bool IsValid() const { return Index != 0xFFFFFFFF; }
		bool operator==(const TextureHandle& other) const { return Index == other.Index; }
	};

	class ITexture {
	public:
		virtual ~ITexture() = default;

		virtual const TextureSpecification& GetSpecification() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual const std::string& GetPath() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual bool IsLoaded() const = 0;

		virtual bool operator==(const ITexture& other) const = 0;
	};

	class ITexture2D : public ITexture {
	public:
		static std::shared_ptr<ITexture2D> Create(const TextureSpecification& specification);
		static std::shared_ptr<ITexture2D> Create(const std::string& path);
	};
	
}
