#include "RmlUiMain.h"
#include <RmlUi/Core.h>
#include <RmlUi/Core/BaseXMLParser.h>
#include <RmlUi/Core/StreamMemory.h>
#include <RmlUi/Core/MeshUtilities.h>
#include <RmlUi/Core/FileInterface.h>
#include <RmlUi/Core/SystemInterface.h>
#include <RmlUi/Core/RenderInterface.h>

#include "RmlUi_Renderer_GL2.h"
#include "RmlUi_Platform_SDL.h"

#include "thirdparty/stb/stb_image.h"
#include <SDL2/SDL.h>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <GL/gl.h>
#include <cstdio>
#include <memory>
#include <algorithm>

// Get SDL window helper (implemented in sdl2glut.cpp)
extern "C" SDL_Window* GfuiGetSdlWindow();

// ============================================================================
// Bitmap Font Engine Implementation (adapted from RmlUi bitmap_font sample)
// ============================================================================

struct BitmapGlyph {
	int advance = 0;
	Rml::Vector2f offset = {0, 0};
	Rml::Vector2f position = {0, 0};
	Rml::Vector2f dimension = {0, 0};
};

using FontGlyphs = Rml::UnorderedMap<Rml::Character, BitmapGlyph>;
using FontKerning = Rml::UnorderedMap<uint64_t, int>;

class FontFaceBitmap {
public:
	FontFaceBitmap(Rml::String family, Rml::Style::FontStyle style, Rml::Style::FontWeight weight, Rml::FontMetrics metrics, Rml::String texture_name, Rml::String texture_path,
		Rml::Vector2f texture_dimensions, FontGlyphs&& glyphs, FontKerning&& kerning) :
		family(family), style(style), weight(weight), metrics(metrics), texture_source(texture_name, texture_path),
		texture_dimensions(texture_dimensions), glyphs(std::move(glyphs)), kerning(std::move(kerning))
	{}

	int GetStringWidth(Rml::StringView string, Rml::Character previous_character) {
		int width = 0;
		for (auto it_char = Rml::StringIteratorU8(string); it_char; ++it_char) {
			Rml::Character character = *it_char;
			auto it_glyph = glyphs.find(character);
			if (it_glyph == glyphs.end())
				continue;
			const BitmapGlyph& glyph = it_glyph->second;
			int kern = GetKerning(previous_character, character);
			width += glyph.advance + kern;
			previous_character = character;
		}
		return width;
	}

	int GenerateString(Rml::RenderManager& render_manager, Rml::StringView string, Rml::Vector2f string_position, Rml::ColourbPremultiplied colour, Rml::TexturedMeshList& mesh_list) {
		int width = 0;
		mesh_list.resize(1);
		mesh_list[0].texture = texture_source.GetTexture(render_manager);

		Rml::Mesh& mesh = mesh_list[0].mesh;
		auto& vertices = mesh.vertices;
		auto& indices = mesh.indices;
		vertices.reserve(string.size() * 4);
		indices.reserve(string.size() * 6);

		Rml::Vector2f position = string_position.Round();
		Rml::Character previous_character = Rml::Character::Null;

		for (auto it_char = Rml::StringIteratorU8(string); it_char; ++it_char) {
			Rml::Character character = *it_char;
			auto it_glyph = glyphs.find(character);
			if (it_glyph == glyphs.end())
				continue;
			int kern = GetKerning(previous_character, character);
			width += kern;
			position.x += kern;

			const BitmapGlyph& glyph = it_glyph->second;
			vertices.resize(vertices.size() + 4);
			indices.resize(indices.size() + 6);

			Rml::Vector2f uv_top_left = glyph.position / texture_dimensions;
			Rml::Vector2f uv_bottom_right = (glyph.position + glyph.dimension) / texture_dimensions;

			Rml::MeshUtilities::GenerateQuad(mesh, Rml::Vector2f(position + glyph.offset).Round(), glyph.dimension, colour, uv_top_left, uv_bottom_right);

			width += glyph.advance;
			position.x += glyph.advance;
			previous_character = character;
		}
		return width;
	}

	const Rml::FontMetrics& GetMetrics() const { return metrics; }
	const Rml::String& GetFamily() const { return family; }
	Rml::Style::FontStyle GetStyle() const { return style; }
	Rml::Style::FontWeight GetWeight() const { return weight; }

private:
	int GetKerning(Rml::Character left, Rml::Character right) const {
		uint64_t key = (((uint64_t)left << 32) | (uint64_t)right);
		auto it = kerning.find(key);
		if (it != kerning.end())
			return it->second;
		return 0;
	}

	Rml::String family;
	Rml::Style::FontStyle style;
	Rml::Style::FontWeight weight;
	Rml::FontMetrics metrics;
	Rml::TextureSource texture_source;
	Rml::Vector2f texture_dimensions;
	FontGlyphs glyphs;
	FontKerning kerning;
};

class FontParserBitmap : public Rml::BaseXMLParser {
public:
	void HandleElementStart(const Rml::String& name, const Rml::XMLAttributes& attributes) override {
		if (name == "info") {
			family = Rml::StringUtilities::ToLower(Get(attributes, "face", Rml::String()));
			metrics.size = Get(attributes, "size", 0);
			style = Get(attributes, "italic", 0) == 1 ? Rml::Style::FontStyle::Italic : Rml::Style::FontStyle::Normal;
			weight = Get(attributes, "bold", 0) == 1 ? Rml::Style::FontWeight::Bold : Rml::Style::FontWeight::Normal;
		} else if (name == "common") {
			metrics.line_spacing = Get(attributes, "lineHeight", 0.f);
			metrics.ascent = Get(attributes, "base", 0.f);
			metrics.descent = metrics.line_spacing - metrics.ascent;
			texture_dimensions.x = Get(attributes, "scaleW", 0.f);
			texture_dimensions.y = Get(attributes, "scaleH", 0.f);
		} else if (name == "page") {
			int id = Get(attributes, "id", -1);
			if (id == 0) {
				texture_name = Get(attributes, "file", Rml::String());
			}
		} else if (name == "char") {
			Rml::Character character = (Rml::Character)Get(attributes, "id", 0);
			if (character != Rml::Character::Null) {
				BitmapGlyph& glyph = glyphs[character];
				glyph.offset.x = Get(attributes, "xoffset", 0.f);
				glyph.offset.y = Get(attributes, "yoffset", 0.f) - metrics.ascent;
				glyph.advance = Get(attributes, "xadvance", 0);
				glyph.position.x = Get(attributes, "x", 0.f);
				glyph.position.y = Get(attributes, "y", 0.f);
				glyph.dimension.x = Get(attributes, "width", 0.f);
				glyph.dimension.y = Get(attributes, "height", 0.f);
				if (character == (Rml::Character)'x')
					metrics.x_height = glyph.dimension.y;
			}
		} else if (name == "kerning") {
			uint64_t first = (uint64_t)Get(attributes, "first", 0);
			uint64_t second = (uint64_t)Get(attributes, "second", 0);
			int amount = Get(attributes, "amount", 0);
			if (first != 0 && second != 0 && amount != 0) {
				uint64_t key = ((first << 32) | second);
				kerning[key] = amount;
			}
		}
	}
	void HandleElementEnd(const Rml::String&) override {}
	void HandleData(const Rml::String&, Rml::XMLDataType) override {}

	Rml::String family;
	Rml::Style::FontStyle style = Rml::Style::FontStyle::Normal;
	Rml::Style::FontWeight weight = Rml::Style::FontWeight::Normal;
	Rml::String texture_name;
	Rml::Vector2f texture_dimensions = {0, 0};
	Rml::FontMetrics metrics = {};
	FontGlyphs glyphs;
	FontKerning kerning;
};

namespace FontProviderBitmap {
	static Rml::Vector<Rml::UniquePtr<FontFaceBitmap>> fonts;
	void Initialise() {}
	void Shutdown() { fonts.clear(); }
	bool LoadFontFace(const Rml::String& file_name) {
		Rml::UniquePtr<Rml::byte[]> data;
		size_t length = 0;
		{
			auto file_interface = Rml::GetFileInterface();
			auto handle = file_interface->Open(file_name);
			if (!handle) return false;
			length = file_interface->Length(handle);
			data.reset(new Rml::byte[length]);
			size_t read_length = file_interface->Read(data.get(), length, handle);
			file_interface->Close(handle);
			if (read_length != length || !data) return false;
		}

		FontParserBitmap parser;
		{
			auto stream = Rml::MakeUnique<Rml::StreamMemory>(data.get(), length);
			stream->SetSourceURL(file_name);
			parser.Parse(stream.get());
			if (parser.family.empty() || parser.glyphs.empty() || parser.texture_name.empty() || parser.metrics.size == 0)
				return false;
			parser.metrics.underline_position = 3.f;
			parser.metrics.underline_thickness = 1.f;
		}
		fonts.push_back(Rml::MakeUnique<FontFaceBitmap>(parser.family, parser.style, parser.weight, parser.metrics, parser.texture_name, file_name,
			parser.texture_dimensions, std::move(parser.glyphs), std::move(parser.kerning)));
		return true;
	}

	FontFaceBitmap* GetFontFaceHandle(const Rml::String& family, Rml::Style::FontStyle style, Rml::Style::FontWeight weight, int size) {
		FontFaceBitmap* best_match = nullptr;
		int best_score = 0;
		for (const auto& font : fonts) {
			int score = 1;
			if (font->GetFamily() == family) score += 100;
			score += 10 - std::min(10, std::abs(font->GetMetrics().size - size));
			if (font->GetStyle() == style) score += 2;
			if (font->GetWeight() == weight) score += 1;
			if (score > best_score) {
				best_match = font.get();
				best_score = score;
			}
		}
		return best_match;
	}
}

class FontEngineInterfaceBitmap : public Rml::FontEngineInterface {
public:
	void Initialize() override { FontProviderBitmap::Initialise(); }
	void Shutdown() override { FontProviderBitmap::Shutdown(); }
	bool LoadFontFace(const Rml::String& file_name, bool, Rml::Style::FontWeight) override {
		return FontProviderBitmap::LoadFontFace(file_name);
	}
	bool LoadFontFace(Rml::Span<const Rml::byte>, const Rml::String& font_family, Rml::Style::FontStyle, Rml::Style::FontWeight, bool) override {
		if (font_family == "rmlui-debugger-font") return true;
		return false;
	}
	Rml::FontFaceHandle GetFontFaceHandle(const Rml::String& family, Rml::Style::FontStyle style, Rml::Style::FontWeight weight, int size) override {
		auto handle = FontProviderBitmap::GetFontFaceHandle(family, style, weight, size);
		return reinterpret_cast<Rml::FontFaceHandle>(handle);
	}
	Rml::FontEffectsHandle PrepareFontEffects(Rml::FontFaceHandle, const Rml::FontEffectList&) override { return 0; }
	const Rml::FontMetrics& GetFontMetrics(Rml::FontFaceHandle handle) override {
		auto handle_bitmap = reinterpret_cast<FontFaceBitmap*>(handle);
		return handle_bitmap->GetMetrics();
	}
	int GetStringWidth(Rml::FontFaceHandle handle, Rml::StringView string, const Rml::TextShapingContext&, Rml::Character prior_character) override {
		auto handle_bitmap = reinterpret_cast<FontFaceBitmap*>(handle);
		return handle_bitmap->GetStringWidth(string, prior_character);
	}
	int GenerateString(Rml::RenderManager& render_manager, Rml::FontFaceHandle handle, Rml::FontEffectsHandle, Rml::StringView string,
		Rml::Vector2f position, Rml::ColourbPremultiplied colour, float, const Rml::TextShapingContext&, Rml::TexturedMeshList& mesh_list) override {
		auto handle_bitmap = reinterpret_cast<FontFaceBitmap*>(handle);
		return handle_bitmap->GenerateString(render_manager, string, position, colour, mesh_list);
	}
	int GetVersion(Rml::FontFaceHandle) override { return 0; }
};

// ============================================================================
// Custom RenderInterface with STB Image Loader for PNG support
// ============================================================================

class CustomRenderInterface : public RenderInterface_GL2 {
public:
	void BeginFrame() {
		// Save existing OpenGL state to prevent glitches with game rendering
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
		RenderInterface_GL2::BeginFrame();
	}

	void EndFrame() {
		RenderInterface_GL2::EndFrame();
		glPopClientAttrib();
		glPopAttrib();
	}

	Rml::TextureHandle LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source) override {
		Rml::FileInterface* file_interface = Rml::GetFileInterface();
		Rml::FileHandle file_handle = file_interface->Open(source);
		if (!file_handle) return 0;

		file_interface->Seek(file_handle, 0, SEEK_END);
		size_t buffer_size = file_interface->Tell(file_handle);
		file_interface->Seek(file_handle, 0, SEEK_SET);

		std::unique_ptr<unsigned char[]> buffer(new unsigned char[buffer_size]);
		file_interface->Read(buffer.get(), buffer_size, file_handle);
		file_interface->Close(file_handle);

		int w, h, channels;
		unsigned char* pixels = stbi_load_from_memory((const stbi_uc*)buffer.get(), (int)buffer_size, &w, &h, &channels, 4);
		if (!pixels) return 0;

		// Convert to premultiplied alpha
		for (int i = 0; i < w * h * 4; i += 4) {
			unsigned char alpha = pixels[i + 3];
			pixels[i + 0] = (pixels[i + 0] * alpha) / 255;
			pixels[i + 1] = (pixels[i + 1] * alpha) / 255;
			pixels[i + 2] = (pixels[i + 2] * alpha) / 255;
		}

		texture_dimensions.x = w;
		texture_dimensions.y = h;

		Rml::TextureHandle handle = GenerateTexture({(const Rml::byte*)pixels, (size_t)(w * h * 4)}, texture_dimensions);
		stbi_image_free(pixels);
		return handle;
	}
};

// ============================================================================
// RmlUiMain Interface & Context Control Logic
// ============================================================================

namespace RmlUiMain {
	static SystemInterface_SDL* g_system_interface = nullptr;
	static CustomRenderInterface* g_render_interface = nullptr;
	static FontEngineInterfaceBitmap* g_font_interface = nullptr;
	static Rml::Context* g_context = nullptr;
	static bool g_active = false;
	static Rml::ElementDocument* g_current_document = nullptr;

	bool Init() {
		if (g_context) return true;
		SDL_Window* window = GfuiGetSdlWindow();
		if (!window) return false;

		g_system_interface = new SystemInterface_SDL();
		g_system_interface->SetWindow(window);

		g_render_interface = new CustomRenderInterface();
		g_font_interface = new FontEngineInterfaceBitmap();

		Rml::SetSystemInterface(g_system_interface);
		Rml::SetRenderInterface(g_render_interface);
		Rml::SetFontEngineInterface(g_font_interface);

		if (!Rml::Initialise()) return false;

		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		g_render_interface->SetViewport(w, h);

		g_context = Rml::CreateContext("main", Rml::Vector2i(w, h));
		if (!g_context) return false;

		// Load bitmap font face
		Rml::LoadFontFace("data/menu/Comfortaa_Regular_22.fnt");

		return true;
	}

	void Shutdown() {
		if (g_context) {
			Rml::Shutdown();
			g_context = nullptr;
		}
		delete g_system_interface; g_system_interface = nullptr;
		delete g_render_interface; g_render_interface = nullptr;
		delete g_font_interface; g_font_interface = nullptr;
	}

	void UpdateAndRender(int screenWidth, int screenHeight) {
		if (!g_context || !g_active) return;

		g_render_interface->SetViewport(screenWidth, screenHeight);
		g_context->SetDimensions(Rml::Vector2i(screenWidth, screenHeight));

		g_context->Update();
		g_render_interface->BeginFrame();
		g_context->Render();
		g_render_interface->EndFrame();
	}

	bool ProcessEvent(void* sdlEvent) {
		if (!g_context || !g_active) return false;
		// Returns true if event is consumed by RmlUi (we return true to block standard GLUT dispatcher)
		return !RmlSDL::InputEventHandler(g_context, *(SDL_Event*)sdlEvent);
	}

	bool IsActive() { return g_active; }
	void SetActive(bool active) { g_active = active; }

	// Custom Event Listener to route HTML clicks to game actions
	class MenuEventListener : public Rml::EventListener {
	public:
		void ProcessEvent(Rml::Event& event) override {
			Rml::String id = event.GetTargetElement()->GetId();
			if (event.GetType() == "click") {
				if (id == "btn-quickrace") {
					// Launch Quick Race
					SetActive(false);
					// Set event in TORCS (e.g. simulate pressing the quick race button)
					// The screen module normally activates screens via GfuiScreenActivate.
				} else if (id == "btn-settings") {
					LoadRmlMenu("data/menu/settings.html");
				} else if (id == "btn-back") {
					LoadRmlMenu("data/menu/main.html");
				} else if (id == "btn-quit") {
					// Gracefully quit SDL2 window loop
					SDL_Event quit_ev;
					quit_ev.type = SDL_QUIT;
					SDL_PushEvent(&quit_ev);
				}
			}
		}
	};
	static MenuEventListener g_menu_listener;

	void LoadRmlMenu(const char* filename) {
		if (!g_context) return;
		if (g_current_document) {
			g_current_document->Close();
			g_current_document = nullptr;
		}
		g_current_document = g_context->LoadDocument(filename);
		if (g_current_document) {
			g_current_document->Show();
			// Add listeners to buttons
			Rml::ElementList buttons;
			g_current_document->GetElementsByTagName(buttons, "button");
			for (auto* btn : buttons) {
				btn->AddEventListener(Rml::EventId::Click, &g_menu_listener);
			}
		}
	}
}
