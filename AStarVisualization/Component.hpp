#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <functional>

enum class ComponentType {
	SHAPE,
	TEXT,
	CLICKABLE
};

struct CShape
{
	sf::VertexArray vertexArray;
	sf::RenderStates states {sf::RenderStates::Default};
	CShape(const sf::VertexArray& vertexArray_)
		: vertexArray(vertexArray_) {}
	CShape(const sf::VertexArray& vertexArray_, const sf::RenderStates& states_)
		: vertexArray(vertexArray_), states(states_) {}
};

struct CText
{
	sf::Text text;
	sf::RenderStates states {sf::RenderStates::Default};
	bool canEdit{ true };
	CText(const sf::String& initialText, const sf::Font& font,
		unsigned int fontSize) 
		: text(initialText, font, fontSize) {}
	CText(const sf::String& initialText, const sf::Font& font,
		unsigned int fontSize, const sf::RenderStates& states_)
		: text(initialText, font, fontSize), states(states_) {}
};

struct CClickable
{
	bool isActive{ true };
	std::function<void()> onClickListener;
	CClickable(std::function<void()> listener) : onClickListener(listener) {}
};