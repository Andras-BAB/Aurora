#pragma once
#include "Aurora/Renderer/Renderer.h"
#include "Aurora/Core/Log.h"

namespace Aurora {

	enum class ShaderDataType {
		None = 0,
		Float,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4,
		Int,
		Int2,
		Int3,
		Int4,
		Bool
	};

	static uint32_t ShaderDataTypeAlignment(ShaderDataType type) {
		switch (type) {
		case ShaderDataType::Float:   return 4;
		case ShaderDataType::Float2:  return 8;
		case ShaderDataType::Float3:  return 4; // scalar layout: csak 3*float
		case ShaderDataType::Float4:  return 4; // scalar layout: 4*float
		case ShaderDataType::Mat3:    return 4; // 3x3 float
		case ShaderDataType::Mat4:    return 4; // 4x4 float
		case ShaderDataType::Int:     return 4;
		case ShaderDataType::Int2:    return 8;
		case ShaderDataType::Int3:    return 4;
		case ShaderDataType::Int4:    return 4;
		case ShaderDataType::Bool:    return 4; // bool is 4 byte a GPU-n
		default: return 4;
		}
	}
	
	static uint32_t ShaderDataTypeAlignmentStd140(ShaderDataType type) {
		switch (type) {
		case ShaderDataType::Float:   return 4;
		case ShaderDataType::Float2:  return 8;
		case ShaderDataType::Float3:  return 16; // std140: vec3 is 16-ra igazítva
		case ShaderDataType::Float4:  return 16;
		case ShaderDataType::Mat3:    return 16; // 3 oszlop, mind 16-ra igazítva
		case ShaderDataType::Mat4:    return 16;
		case ShaderDataType::Int:     return 4;
		case ShaderDataType::Int2:    return 8;
		case ShaderDataType::Int3:    return 16;
		case ShaderDataType::Int4:    return 16;
		case ShaderDataType::Bool:    return 4;
		default: return 4;
		}
	}
	
	static size_t Align(size_t offset, size_t alignment) {
		return (offset + alignment - 1) & ~(alignment - 1);
	}

	static uint32_t ShaderDataTypeSize(ShaderDataType type) {
		switch (type) {
		case ShaderDataType::Float:    return 4;
		case ShaderDataType::Float2:   return 4 * 2;
		case ShaderDataType::Float3:   return 4 * 3;
		case ShaderDataType::Float4:   return 4 * 4;
		case ShaderDataType::Mat3:     return 4 * 3 * 3;
		case ShaderDataType::Mat4:     return 4 * 4 * 4;
		case ShaderDataType::Int:      return 4;
		case ShaderDataType::Int2:     return 4 * 2;
		case ShaderDataType::Int3:     return 4 * 3;
		case ShaderDataType::Int4:     return 4 * 4;
		case ShaderDataType::Bool:	   return 4;
		default: return 4;
		}
		AU_CORE_ERROR("Unknown ShaderDataType!");
		return 0;
	}

	struct BufferElement {
		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		size_t Offset;
		bool Normalized;

		BufferElement() = default;

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized) {
		}

		uint32_t GetComponentCount() const {
			switch (Type) {
			case ShaderDataType::Float:   return 1;
			case ShaderDataType::Float2:  return 2;
			case ShaderDataType::Float3:  return 3;
			case ShaderDataType::Float4:  return 4;
			case ShaderDataType::Mat3:    return 3; // 3* float3
			case ShaderDataType::Mat4:    return 4; // 4* float4
			case ShaderDataType::Int:     return 1;
			case ShaderDataType::Int2:    return 2;
			case ShaderDataType::Int3:    return 3;
			case ShaderDataType::Int4:    return 4;
			case ShaderDataType::Bool:    return 1;
			default: return 0;
			}

			AU_CORE_ERROR("Unknown ShaderDataType!");
			return 0;
		}
	};

	enum class LayoutType {
		Scalar,
		Std140
	};

	class BufferLayout {
	public:
		BufferLayout() = default;

		BufferLayout(std::initializer_list<BufferElement> elements)
			: m_Elements(elements) {
			CalculateOffsetsAndStride();
		}

		uint32_t GetStride() const { return m_Stride; }
		const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
	private:
		void CalculateOffsetsAndStride() {
			size_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements) {
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}

		void CalculateOffsetsAndStrideScalar() {
			size_t offset = 0;
			m_Stride = 0;
		
			for (auto& element : m_Elements) {
				size_t align = ShaderDataTypeAlignment(element.Type);
				offset = Align(offset, align);
				element.Offset = offset;
				offset += element.Size;
			}
		
			m_Stride = static_cast<uint32_t>(Align(offset, 4)); // végső stride igazítása
		}
		
		void CalculateOffsetsAndStrideStd140() {
			size_t offset = 0;
			m_Stride = 0;
		
			for (auto& element : m_Elements) {
				size_t align = ShaderDataTypeAlignmentStd140(element.Type);
				offset = Align(offset, align);
				element.Offset = offset;
		
				// Mátrixoknál std140-ben minden oszlop külön vektorként van tárolva
				if (element.Type == ShaderDataType::Mat3) {
					offset += 3 * 16; // 3 oszlop, mind 16 byte-ra igazítva
				}
				else if (element.Type == ShaderDataType::Mat4) {
					offset += 4 * 16; // 4 oszlop, mind 16 byte-ra igazítva
				}
				else {
					offset += element.Size;
				}
			}
		
			m_Stride = static_cast<uint32_t>(Align(offset, 16)); // std140 végén 16 byte igazítás
		}
	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};

	class VertexBuffer {
	public:
		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

		static std::shared_ptr<VertexBuffer> Create(uint32_t size);
		static std::shared_ptr<VertexBuffer> Create(float* vertices, uint32_t size);
	};

	// Currently only supports 32-bit index buffers
	class IndexBuffer {
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;
		
		virtual uint32_t GetCount() const = 0;

		static std::shared_ptr<IndexBuffer> Create(uint32_t size);
		static std::shared_ptr<IndexBuffer> Create(uint32_t* indices, uint32_t count);
	};

}
