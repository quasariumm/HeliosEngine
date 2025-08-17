#pragma once

/*
 *	A header for common display functions
 *	for use with the AssignProperty function from the Component class
 */


namespace Engine
{

struct ComponentProperty;


void DisplayNone(const ComponentProperty* property);

/**
 * @brief Displays the property as a 3 component RGB color
 */
void DisplayColorRGB(const ComponentProperty* property);

/**
 * @brief Displays the property as a 3 component RGB color (allows for HDR colors)
 */
void DisplayColorRGB_HDR(const ComponentProperty* property);

/**
 * @brief Displays the property as a 3 component HSV color
 */
void DisplayColorHSV(const ComponentProperty* property);

/**
 * @brief Displays the property as a 3 component HSV color (allows for HDR colors)
 */
void DisplayColorHSV_HDR(const ComponentProperty* property);

/**
 * @brief Displays the property as a 4 component RGBA color
 */
void DisplayColorRGBA(const ComponentProperty* property);

/**
 * @brief Displays the property as a 4 component RGBA color (allows for HDR colors)
 */
void DisplayColorRGBA_HDR(const ComponentProperty* property);

/**
 * @brief Displays the property as a 4 component HSVA color
 */
void DisplayColorHSVA(const ComponentProperty* property);

/**
 * @brief Displays the property as a 4 component HSVA color (allows for HDR colors)
 */
void DisplayColorHSVA_HDR(const ComponentProperty* property);

}
