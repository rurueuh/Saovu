#pragma once

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "ImGuiLayer.h"
#include "Renderer.h"
#include "ImGuiItem.h"
#include <vector>
#include <functional>
#include <iostream>
#include <algorithm>

/**
 * @class TextItem
 * @brief Represents a text item in the ImGui interface.
 */
class TextItem : public ImGuiItem {
public:
	/**
	 * @brief Constructs a new TextItem object.
	 * @param fmt The format string for the text.
	 * @param ... The arguments for the format string.
	 */
	TextItem(const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		vsnprintf_s(buffer, sizeof(buffer), fmt, args);
		va_end(args);
	}

	/**
	 * @brief Draws the text item.
	 */
	void DrawImGui() override {
		ImGui::TextUnformatted(buffer);
	}

	/**
	 * @brief Sets the text of the item.
	 * @param fmt The format string for the text.
	 * @param ... The arguments for the format string.
	 */
	void setText(const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		vsnprintf_s(buffer, sizeof(buffer), fmt, args);
		va_end(args);
	}
private:
	char buffer[256];
};

/**
 * @class ButtonItem
 * @brief Represents a button item in the ImGui interface.
 */
class ButtonItem : public ImGuiItem {
public:
	/**
	 * @brief Constructs a new ButtonItem object.
	 * @param lbl The label of the button.
	 * @param cb The callback function to execute when the button is clicked.
	 */
	ButtonItem(const char* lbl, std::function<void()> cb) : label(lbl), callback(cb) {}

	/**
	 * @brief Draws the button item.
	 */
	void DrawImGui() override {
		if (ImGui::Button(label)) {
			callback();
		}
	}
private:
	const char* label;
	std::function<void()> callback;
};

/**
 * @class SeparatorItem
 * @brief Represents a separator item in the ImGui interface.
 */
class SeparatorItem : public ImGuiItem {
public:
	/**
	 * @brief Draws the separator item.
	 */
	void DrawImGui() override {
		ImGui::Separator();
	}
};

/**
 * @class FPSItem
 * @brief Represents an FPS counter item in the ImGui interface.
 */
class FPSItem : public ImGuiItem {
public:
	/**
	 * @brief Draws the FPS counter item.
	 */
	void DrawImGui() override {
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	}
};

/**
 * @class SliderFloatItem
 * @brief Represents a slider for float values in the ImGui interface.
 */
class SliderFloatItem : public ImGuiItem {
public:
	/**
	 * @brief Constructs a new SliderFloatItem object.
	 * @param lbl The label of the slider.
	 * @param val A pointer to the float value.
	 * @param mn The minimum value of the slider.
	 * @param mx The maximum value of the slider.
	 * @param o The callback function to execute when the value changes.
	 */
	SliderFloatItem(const char* lbl, float* val, float mn, float mx, std::function<void(float)> o)
		: label(lbl), value(val), onChange(std::move(o)) {
		min = mn; max = mx;
		if (!value) {
			value = &dummy;
		}
	}

	/**
	 * @brief Draws the slider item.
	 */
	void DrawImGui() override {
		if (ImGui::SliderFloat(label, value, min, max)) {
			if (onChange) onChange(*value);
		}
	}

private:
	const char* label;
	std::function<void(float)> onChange;
	float dummy = 0.0f;
	float* value;
	float min;
	float max;
};

/**
 * @class ImGuiDx12
 * @brief Manages the ImGui interface for DirectX 12.
 */
class ImGuiDx12
{
public:
	/**
	 * @brief Draws the ImGui interface.
	 * @param renderer The renderer to use for drawing.
	 */
	void Draw(Renderer& );

	/**
	 * @brief Initializes the ImGui interface.
	 * @param hwnd The window handle.
	 * @param gd The graphics device.
	 * @param sc The swap chain.
	 */
	void Init(HWND hwnd, GraphicsDevice& gd, SwapChain& sc) {
		m_imgui.Init(hwnd, gd, sc);
	}

	/**
	 * @brief Starts a new ImGui frame.
	 */
	void NewFrame() {
		m_imgui.NewFrame();
	}

	/**
	 * @brief Adds a button to the ImGui interface.
	 * @param label The label of the button.
	 * @param onClick The callback function to execute when the button is clicked.
	 * @return A shared pointer to the created button item.
	 */
	std::shared_ptr<ButtonItem> AddButton(const char* label, std::function<void()> onClick) {
		
		auto t = AddItem<ButtonItem>(label, onClick);
		return t;
	}

	/**
	 * @brief Adds a separator to the ImGui interface.
	 * @return A shared pointer to the created separator item.
	 */
	std::shared_ptr<SeparatorItem> addSeparator() {
		
		auto t = AddItem<SeparatorItem>();
		return t;
	}

	/**
	 * @brief Adds text to the ImGui interface.
	 * @param fmt The format string for the text.
	 * @param ... The arguments for the format string.
	 * @return A shared pointer to the created text item.
	 */
	std::shared_ptr<TextItem> addText(const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		auto t = AddItem<TextItem>(fmt, args);
		va_end(args);
		return t;
	}

	/**
	 * @brief Adds an FPS counter to the ImGui interface.
	 * @return A shared pointer to the created FPS item.
	 */
	std::shared_ptr<FPSItem> addFPS() {
		
		auto t = AddItem<FPSItem>();
		return t;
	}

	/**
	 * @brief Adds a slider for float values to the ImGui interface.
	 * @param label The label of the slider.
	 * @param value A pointer to the float value.
	 * @param min The minimum value of the slider.
	 * @param max The maximum value of the slider.
	 * @param onChange The callback function to execute when the value changes.
	 * @return A shared pointer to the created slider item.
	 */
	std::shared_ptr<SliderFloatItem> addSliderFloat(const char* label, float* value, float min, float max, std::function<void(float)> onChange = nullptr) {
		
		auto t = AddItem<SliderFloatItem>(label, value, min, max, onChange);
		return t;
	}

	/**
	 * @brief Adds a generic item to the ImGui interface.
	 * @tparam T The type of the item to add.
	 * @tparam Args The types of the arguments for the item's constructor.
	 * @param args The arguments for the item's constructor.
	 * @return A shared pointer to the created item.
	 */
	template<typename T, typename... Args>
	std::shared_ptr<T> AddItem(Args&&... args) {
		assert((std::is_base_of_v<ImGuiItem, T>));
		auto t = std::make_shared<T>(std::forward<Args>(args)...);
		m_items.push_back(t);
		return t;
	}
private:
	ImGuiLayer m_imgui;

	std::vector<std::shared_ptr<ImGuiItem>> m_items;
};

