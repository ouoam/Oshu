#pragma once

#include <vector>
#include <queue>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Easing.h"


namespace Object {

namespace Animate {

enum doChange {
	Position,
	Scale,
	Origin,
	Rotation,
	Remove,
	Color,
	Opacity
};

struct toChange {
	doChange todo;
	uint32_t duration;
	Easing easing;
	union changeTo{
		sf::Vector2f position;
		sf::Vector2f origin;
		sf::Color color;
		float scale;
		float angle;
		int16_t opacity;

		changeTo() {}
	} to;

	toChange(doChange T, uint32_t D, Easing E ) :
		todo(T), duration(D), easing(E) {}
};

class Animate : public sf::Transformable {
public:
	Animate();

	virtual void update();
	Animate &moveTo			(sf::Vector2f position, uint32_t duration = 0, Easing easing = None);
	Animate &moveOriginTo	(sf::Vector2f origin,	uint32_t duration = 0, Easing easing = None);
	Animate &scaleTo		(float scale,			uint32_t duration = 0, Easing easing = None);
	Animate &rotateTo		(float angle,			uint32_t duration = 0, Easing easing = None);
	Animate &then();
	Animate &expire();
	Animate &setScaleFromNow();

	bool willBeRemove;
		
protected:
	sf::Clock stTime;
	std::queue<std::vector<toChange>> timeline;

	bool haveUpdate;

	void addEvent(toChange toAdd);
	sf::Mutex Mutex;

private:
	sf::Vector2f stPosition;
	sf::Vector2f stOrigin;
	float relativeScale;
	float stScale;
	float stAngle;
};

class AnimeSprite : public Animate, public virtual sf::Drawable {
public:

	AnimeSprite();
	explicit AnimeSprite(const sf::Texture& texture);

	virtual void update();

	AnimeSprite &colorTo(sf::Color color, uint32_t duration = 0, Easing easing = None);
	AnimeSprite &fadeTo(uint8_t opacity, uint32_t duration = 0, Easing easing = None);

	void setTexture(const sf::Texture& texture, bool resetRect = false);
	void setTextureRect(const sf::IntRect& rectangle);
	void setColor(const sf::Color& color);

	const sf::Texture* getTexture() const;
	const sf::Color& getColor() const;
	sf::FloatRect getLocalBounds() const;

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	void updatePositions();
	void updateTexCoords();

	sf::Color stColor;
	int16_t stOpacity;

	sf::Vertex         m_vertices[4];
	const sf::Texture* m_texture;
	sf::IntRect        m_textureRect;
};

class AnimeShape : public Animate, public virtual sf::Drawable {
public:
	AnimeShape();
	void setTexture(const sf::Texture* texture, bool resetRect = false);
	void setTextureRect(const sf::IntRect& rect);
	void setFillColor(const sf::Color& color);
	void setOutlineColor(const sf::Color& color);
	void setOutlineThickness(float thickness);
	const sf::Texture* getTexture() const;
	const sf::IntRect& getTextureRect() const;
	const sf::Color& getFillColor() const;
	const sf::Color& getOutlineColor() const;
	float getOutlineThickness() const;
	//virtual std::size_t getPointCount() const = 0;
	//virtual sf::Vector2f getPoint(std::size_t index) const = 0;
	sf::FloatRect getLocalBounds() const;
	sf::FloatRect getGlobalBounds() const;

	virtual void update();

	AnimeShape &colorTo(sf::Color color, uint32_t duration = 0, Easing easing = None);
	AnimeShape &fadeTo(uint8_t opacity, uint32_t duration = 0, Easing easing = None);

	void setVertex(sf::VertexArray vertexs);

protected:
	void updateIn();

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void updateFillColors();
	void updateTexCoords();
	void updateOutline();
	void updateOutlineColors();

private:
	const sf::Texture* m_texture;          ///< Texture of the shape
	sf::IntRect        m_textureRect;      ///< Rectangle defining the area of the source texture to display
	sf::Color          m_fillColor;        ///< Fill color
	sf::Color          m_outlineColor;     ///< Outline color
	float              m_outlineThickness; ///< Thickness of the shape's outline
	sf::VertexArray    m_vertices;         ///< Vertex array containing the fill geometry
	sf::VertexArray    m_outlineVertices;  ///< Vertex array containing the outline geometry
	sf::FloatRect      m_insideBounds;     ///< Bounding rectangle of the inside (fill)
	sf::FloatRect      m_bounds;           ///< Bounding rectangle of the whole shape (outline + fill)

	sf::Color stColor;
	int16_t stOpacity;
};

} //namespace Animate

} //namespace Object