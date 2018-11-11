#include "Animate.h"
#include <iostream>

namespace Object {

namespace Animate {

Animate::Animate()
{
	willBeRemove = false;
	haveUpdate = false;

	stPosition = getPosition();
	stScale = (getScale().x + getScale().y) / 2.0;
	stOrigin = getOrigin();
	stAngle = getRotation();

	relativeScale = 1;
}

void Animate::update() {
	haveUpdate = true;

	Mutex.lock();

	while (!timeline.empty()) {
		uint32_t elapsed = stTime.getElapsedTime().asMilliseconds();

		std::vector<toChange> *front;

		front = &timeline.front();

		if (!(*front).empty()) {
			std::vector<toChange>::iterator it = (*front).begin();
			while (it != (*front).end()) {
				if (it->duration <= elapsed) {
					switch (it->todo) {
					case Position:
						setPosition(it->to.position);
						stPosition = it->to.position;
						break;
					case Scale:
						setScale(sf::Vector2f(it->to.scale, it->to.scale));
						stScale = it->to.scale;
						break;
					case Origin:
						setOrigin(it->to.origin);
						stOrigin = it->to.origin;
						break;
					case Rotation:
						setRotation(it->to.angle);
						stAngle = it->to.angle;
						break;
					case Remove:
						willBeRemove = true;
						break;
					}

					// Thank https://stackoverflow.com/questions/15042942/vector-iterators-incompatible-while-erase-from-vector
					it = (*front).erase(it);

					if (it != (*front).begin())
					{
						it = std::prev(it);
					}
							
				} else {
					float tempScale;
					switch (it->todo) {
					case Position:
						setPosition(doApply(stPosition, it->to.position, elapsed, it->duration, it->easing));
						break;
					case Scale:
						tempScale = doApply(stScale, it->to.scale, elapsed, it->duration, it->easing);
						setScale(sf::Vector2f(tempScale, tempScale));
						break;
					case Origin:
						setOrigin(doApply(stOrigin, it->to.origin, elapsed, it->duration, it->easing));
						break;
					case Rotation:
						setRotation(doApply(stAngle, it->to.angle, elapsed, it->duration, it->easing));
						break;
					case Remove:
						willBeRemove = true;
						break;
					}

					++it;
				}
			}
		}

		if (timeline.front().empty()) {
			stTime.restart();
			timeline.pop();
		} else {
			break;
		}
	}

	Mutex.unlock();
}

void Animate::addEvent(toChange toAdd) {
	Mutex.lock();

	if (haveUpdate == true) {
		while (!timeline.empty())
			timeline.pop();
		haveUpdate = false;
	}

	if (timeline.empty()) {
		std::vector<toChange> temp;
		timeline.push(temp);		// For get start value
		timeline.push(temp);		// For push data to
	}

	timeline.back().push_back(toAdd);

	Mutex.unlock();
}

Animate &Animate::moveTo(sf::Vector2f position, uint32_t duration, Easing easing) {
	toChange toPush(Position, duration, easing);
	toPush.to.position = position;

	addEvent(toPush);
	return *this;
}

Animate &Animate::scaleTo(float scale, uint32_t duration, Easing easing) {
	toChange toPush(Scale, duration, easing);
	toPush.to.scale = scale * relativeScale;

	addEvent(toPush);
	return *this;
}

Animate &Animate::moveOriginTo(sf::Vector2f origin, uint32_t duration, Easing easing) {
	toChange toPush(Origin, duration, easing);
	toPush.to.origin = origin;

	addEvent(toPush);
	return *this;
}



Animate &Animate::rotateTo(float angle, uint32_t duration, Easing easing) {
	toChange toPush(Rotation, duration, easing);
	toPush.to.angle = angle;

	addEvent(toPush);
	return *this;
}

Animate &Animate::then() {
	std::vector<toChange> temp;

	Mutex.lock();

	timeline.push(temp);

	Mutex.unlock();

	return *this;
}

Animate &Animate::expire() {
	toChange toPush(Remove, 0, None);

	addEvent(toPush);
	return *this;
}

Animate &Animate::setScaleFromNow() {
	relativeScale = (getScale().x + getScale().y) / 2.0;

	return *this;
}


AnimeSprite::AnimeSprite() : 
	Animate(),
	m_texture(NULL),
	m_textureRect() {}

AnimeSprite::AnimeSprite(const sf::Texture& texture) : 
	Animate(),
	m_texture(NULL),
	m_textureRect() {
	setTexture(texture);
}

void AnimeSprite::update() {
	haveUpdate = true;

	Mutex.lock();

	while (!timeline.empty()) {
		uint32_t elapsed = stTime.getElapsedTime().asMilliseconds();

		std::vector<toChange> *front;

		front = &timeline.front();

		if (!(*front).empty()) {
			std::vector<toChange>::iterator it = (*front).begin();
			while (it != (*front).end()) {
				if (it->duration <= elapsed) {
					bool willErase = false;
					sf::Color tempColor = getColor();

					switch (it->todo) {
					case Color:
						setColor(it->to.color);
						stColor = it->to.color;
						willErase = true;
						break;
					case Opacity:
						tempColor.a = it->to.opacity;
						setColor(tempColor);
						stOpacity = it->to.opacity;
						willErase = true;
						break;
					}

					// Thank https://stackoverflow.com/questions/15042942/vector-iterators-incompatible-while-erase-from-vector
					if (willErase) {
						it = (*front).erase(it);

						if (it != (*front).begin()) {
							it = std::prev(it);
						}
					} else {
						++it;
					}

				} else {
					sf::Color tempColor = getColor();
					switch (it->todo) {
					case Color:
						setColor(doApply(stColor, it->to.color, elapsed, it->duration, it->easing));
						break;
					case Opacity:
						tempColor.a = doApply(stOpacity, it->to.opacity, elapsed, it->duration, it->easing);
						setColor(tempColor);
						break;
					}

					++it;
				}
			}
		}

		if ((*front).empty()) {
			stTime.restart();
			timeline.pop();
		} else {
			break;
		}
	}

	Mutex.unlock();

	Animate::update();
}

AnimeSprite &AnimeSprite::colorTo(sf::Color color, uint32_t duration, Easing easing) {
	toChange toPush(Color, duration, easing);
	toPush.to.color = color;

	addEvent(toPush);
	return *this;
}

AnimeSprite &AnimeSprite::fadeTo(uint8_t opacity, uint32_t duration, Easing easing) {
	toChange toPush(Opacity, duration, easing);
	toPush.to.opacity = opacity;

	addEvent(toPush);
	return *this;
}

////////////////////////////////////////////////////////////
void AnimeSprite::setTexture(const sf::Texture& texture, bool resetRect) {
	// Recompute the texture area if requested, or if there was no valid texture & rect before
	if (resetRect || (!m_texture && (m_textureRect == sf::IntRect())))
		setTextureRect(sf::IntRect(0, 0, texture.getSize().x, texture.getSize().y));

	// Assign the new texture
	m_texture = &texture;

	stColor = getColor();
	stOpacity = getColor().a;

	sf::Vector2u size = texture.getSize();
	setOrigin(size.x / 2.0, size.y / 2.0);
}


////////////////////////////////////////////////////////////
void AnimeSprite::setTextureRect(const sf::IntRect& rectangle) {
	if (rectangle != m_textureRect) {
		m_textureRect = rectangle;
		updatePositions();
		updateTexCoords();
	}
}


////////////////////////////////////////////////////////////
void AnimeSprite::setColor(const sf::Color& color) {
	// Update the vertices' color
	m_vertices[0].color = color;
	m_vertices[1].color = color;
	m_vertices[2].color = color;
	m_vertices[3].color = color;
}


////////////////////////////////////////////////////////////
const sf::Texture* AnimeSprite::getTexture() const {
	return m_texture;
}


////////////////////////////////////////////////////////////
const sf::Color& AnimeSprite::getColor() const {
	return m_vertices[0].color;
}


////////////////////////////////////////////////////////////
sf::FloatRect AnimeSprite::getLocalBounds() const {
	float width = static_cast<float>(std::abs(m_textureRect.width));
	float height = static_cast<float>(std::abs(m_textureRect.height));

	return sf::FloatRect(0.f, 0.f, width, height);
}


////////////////////////////////////////////////////////////
void AnimeSprite::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (m_texture) {
		states.transform *= getTransform();
		states.texture = m_texture;
		target.draw(m_vertices, 4, sf::TriangleStrip, states);
	}
}


////////////////////////////////////////////////////////////
void AnimeSprite::updatePositions() {
	sf::FloatRect bounds = getLocalBounds();

	m_vertices[0].position = sf::Vector2f(0, 0);
	m_vertices[1].position = sf::Vector2f(0, bounds.height);
	m_vertices[2].position = sf::Vector2f(bounds.width, 0);
	m_vertices[3].position = sf::Vector2f(bounds.width, bounds.height);
}


////////////////////////////////////////////////////////////
void AnimeSprite::updateTexCoords() {
	float left = static_cast<float>(m_textureRect.left);
	float right = left + m_textureRect.width;
	float top = static_cast<float>(m_textureRect.top);
	float bottom = top + m_textureRect.height;

	m_vertices[0].texCoords = sf::Vector2f(left, top);
	m_vertices[1].texCoords = sf::Vector2f(left, bottom);
	m_vertices[2].texCoords = sf::Vector2f(right, top);
	m_vertices[3].texCoords = sf::Vector2f(right, bottom);
}

} //namespace Animate

} //namespace Object
