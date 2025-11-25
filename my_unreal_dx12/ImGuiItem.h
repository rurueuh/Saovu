#pragma once

/**
 * @class ImGuiItem
 * @brief Represents an item in the ImGui interface.
 * This is an abstract base class for all ImGui items.
 */
class ImGuiItem
{
public:
	/**
	 * @brief Virtual destructor.
	 */
	virtual ~ImGuiItem() = default;

	/**
	 * @brief Draws the ImGui item.
	 * This is a pure virtual function that must be implemented by derived classes.
	 */
	virtual void DrawImGui() = 0;
};

