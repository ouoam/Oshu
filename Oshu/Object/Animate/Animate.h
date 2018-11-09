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
	////////////////////////////////////////////////////////////
	/// \brief Default constructor
	///
	/// Creates an empty sprite with no source texture.
	///
	////////////////////////////////////////////////////////////
	AnimeSprite();

	////////////////////////////////////////////////////////////
	/// \brief Construct the sprite from a source texture
	///
	/// \param texture Source texture
	///
	/// \see setTexture
	///
	////////////////////////////////////////////////////////////
	explicit AnimeSprite(const sf::Texture& texture);

	virtual void update();

	AnimeSprite &colorTo(sf::Color color, uint32_t duration, Easing easing);

	AnimeSprite &fadeTo(uint8_t opacity, uint32_t duration, Easing easing);


	////////////////////////////////////////////////////////////
	/// \brief Change the source texture of the sprite
	///
	/// The \a texture argument refers to a texture that must
	/// exist as long as the sprite uses it. Indeed, the sprite
	/// doesn't store its own copy of the texture, but rather keeps
	/// a pointer to the one that you passed to this function.
	/// If the source texture is destroyed and the sprite tries to
	/// use it, the behavior is undefined.
	/// If \a resetRect is true, the TextureRect property of
	/// the sprite is automatically adjusted to the size of the new
	/// texture. If it is false, the texture rect is left unchanged.
	///
	/// \param texture   New texture
	/// \param resetRect Should the texture rect be reset to the size of the new texture?
	///
	/// \see getTexture, setTextureRect
	///
	////////////////////////////////////////////////////////////
	void setTexture(const sf::Texture& texture, bool resetRect = false);

	////////////////////////////////////////////////////////////
	/// \brief Set the sub-rectangle of the texture that the sprite will display
	///
	/// The texture rect is useful when you don't want to display
	/// the whole texture, but rather a part of it.
	/// By default, the texture rect covers the entire texture.
	///
	/// \param rectangle Rectangle defining the region of the texture to display
	///
	/// \see getTextureRect, setTexture
	///
	////////////////////////////////////////////////////////////
	void setTextureRect(const sf::IntRect& rectangle);

	////////////////////////////////////////////////////////////
	/// \brief Set the global color of the sprite
	///
	/// This color is modulated (multiplied) with the sprite's
	/// texture. It can be used to colorize the sprite, or change
	/// its global opacity.
	/// By default, the sprite's color is opaque white.
	///
	/// \param color New color of the sprite
	///
	/// \see getColor
	///
	////////////////////////////////////////////////////////////
	void setColor(const sf::Color& color);

	////////////////////////////////////////////////////////////
	/// \brief Get the global color of the sprite
	///
	/// \return Global color of the sprite
	///
	/// \see setColor
	///
	////////////////////////////////////////////////////////////
	const sf::Color& getColor() const;

	////////////////////////////////////////////////////////////
	/// \brief Get the local bounding rectangle of the entity
	///
	/// The returned rectangle is in local coordinates, which means
	/// that it ignores the transformations (translation, rotation,
	/// scale, ...) that are applied to the entity.
	/// In other words, this function returns the bounds of the
	/// entity in the entity's coordinate system.
	///
	/// \return Local bounding rectangle of the entity
	///
	////////////////////////////////////////////////////////////
	sf::FloatRect getLocalBounds() const;

private:
	sf::Color stColor;
	int16_t stOpacity;

	////////////////////////////////////////////////////////////
	/// \brief Draw the sprite to a render target
	///
	/// \param target Render target to draw to
	/// \param states Current render states
	///
	////////////////////////////////////////////////////////////
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	////////////////////////////////////////////////////////////
	/// \brief Update the vertices' positions
	///
	////////////////////////////////////////////////////////////
	void updatePositions();

	////////////////////////////////////////////////////////////
	/// \brief Update the vertices' texture coordinates
	///
	////////////////////////////////////////////////////////////
	void updateTexCoords();

	////////////////////////////////////////////////////////////
	// Member data
	////////////////////////////////////////////////////////////
	sf::Vertex         m_vertices[4]; ///< Vertices defining the sprite's geometry
	const sf::Texture* m_texture;     ///< Texture of the sprite
	sf::IntRect        m_textureRect; ///< Rectangle defining the area of the source texture to display
};

} //namespace Animate

} //namespace Object