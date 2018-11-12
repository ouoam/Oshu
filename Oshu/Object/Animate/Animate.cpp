#include "Animate.h"
#include <iostream>

namespace {
// Compute the normal of a segment
sf::Vector2f computeNormal(const sf::Vector2f& p1, const sf::Vector2f& p2) {
	sf::Vector2f normal(p1.y - p2.y, p2.x - p1.x);
	float length = std::sqrt(normal.x * normal.x + normal.y * normal.y);
	if (length != 0.f)
		normal /= length;
	return normal;
}

// Compute the dot product of two vectors
float dotProduct(const sf::Vector2f& p1, const sf::Vector2f& p2) {
	return p1.x * p2.x + p1.y * p2.y;
}
}

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
					if (willErase) it = (*front).erase(it);
					else ++it;

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


void AnimeShape::update() {
	haveUpdate = true;

	Mutex.lock();

	while (!timeline.empty()) {
		uint32_t elapsed = stTime.getElapsedTime().asMilliseconds();

		std::vector<toChange> *front;

		front = &timeline.front();

		if (!(*front).empty()) {
			std::vector<toChange>::iterator it = (*front).begin();
			while (it != (*front).end()) {
				sf::Color tempColor = getFillColor();
				sf::Color tempColorO = getOutlineColor();
				if (it->duration <= elapsed) {
					bool willErase = false;
					switch (it->todo) {
					case Color:
						setFillColor(it->to.color);
						stColor = it->to.color;
						willErase = true;
						break;
					case Opacity:
						tempColor.a = it->to.opacity;
						tempColorO.a = tempColor.a;
						setFillColor(tempColor);
						setOutlineColor(tempColorO);
						stOpacity = it->to.opacity;
						willErase = true;
						break;
					}

					// Thank https://stackoverflow.com/questions/15042942/vector-iterators-incompatible-while-erase-from-vector
					if (willErase) it = (*front).erase(it);
					else ++it;

				}
				else {
					switch (it->todo) {
					case Color:
						setFillColor(doApply(stColor, it->to.color, elapsed, it->duration, it->easing));
						break;
					case Opacity:
						tempColor.a = doApply(stOpacity, it->to.opacity, elapsed, it->duration, it->easing);
						tempColorO.a = tempColor.a;
						//std::cout << stOpacity << "\t" << it->to.opacity << "\t" << elapsed << "\t" << it->duration << "\t" << tempColor.a << std::endl;
						//std::cout << (int)tempColor.r << "\t" << (int)tempColor.g << "\t" << (int)tempColor.b << "\t" << (int)tempColor.a << std::endl;
						setFillColor(tempColor);
						setOutlineColor(tempColorO);
						break;
					}

					++it;
				}
			}
		}

		if ((*front).empty()) {
			stTime.restart();
			timeline.pop();
		}
		else {
			break;
		}
	}

	Mutex.unlock();

	Animate::update();
}

AnimeShape &AnimeShape::colorTo(sf::Color color, uint32_t duration, Easing easing) {
	toChange toPush(Color, duration, easing);
	toPush.to.color = color;

	addEvent(toPush);
	return *this;
}

AnimeShape &AnimeShape::fadeTo(uint8_t opacity, uint32_t duration, Easing easing) {
	toChange toPush(Opacity, duration, easing);
	toPush.to.opacity = opacity;

	addEvent(toPush);
	return *this;
}

void AnimeShape::setVertex(sf::VertexArray vertexs) {
	// Get the total number of points of the shape
	std::size_t count = vertexs.getVertexCount();
	if (count < 3) {
		m_vertices.resize(0);
		m_outlineVertices.resize(0);
		return;
	}

	m_vertices.resize(count + 2); // + 2 for center and repeated first point

	// Position
	for (std::size_t i = 0; i < count; ++i)
		m_vertices[i + 1] = vertexs[i];
	m_vertices[count + 1].position = m_vertices[1].position;

	// Update the bounding rectangle
	m_vertices[0] = m_vertices[1]; // so that the result of getBounds() is correct
	m_insideBounds = m_vertices.getBounds();

	// Compute the center and make it the first vertex
	m_vertices[0].position.x = m_insideBounds.left + m_insideBounds.width / 2;
	m_vertices[0].position.y = m_insideBounds.top + m_insideBounds.height / 2;
}

////////////////////////////////////////////////////////////
void AnimeShape::setTexture(const sf::Texture* texture, bool resetRect) {
	if (texture) {
		// Recompute the texture area if requested, or if there was no texture & rect before
		if (resetRect || (!m_texture && (m_textureRect == sf::IntRect())))
			setTextureRect(sf::IntRect(0, 0, texture->getSize().x, texture->getSize().y));
	}

	// Assign the new texture
	m_texture = texture;
}


////////////////////////////////////////////////////////////
const sf::Texture* AnimeShape::getTexture() const {
	return m_texture;
}


////////////////////////////////////////////////////////////
void AnimeShape::setTextureRect(const sf::IntRect& rect) {
	m_textureRect = rect;
	updateTexCoords();
}


////////////////////////////////////////////////////////////
const sf::IntRect& AnimeShape::getTextureRect() const {
	return m_textureRect;
}


////////////////////////////////////////////////////////////
void AnimeShape::setFillColor(const sf::Color& color) {
	m_fillColor = color;
	updateFillColors();
}


////////////////////////////////////////////////////////////
const sf::Color& AnimeShape::getFillColor() const {
	return m_fillColor;
}


////////////////////////////////////////////////////////////
void AnimeShape::setOutlineColor(const sf::Color& color) {
	m_outlineColor = color;
	updateOutlineColors();
}


////////////////////////////////////////////////////////////
const sf::Color& AnimeShape::getOutlineColor() const {
	return m_outlineColor;
}


////////////////////////////////////////////////////////////
void AnimeShape::setOutlineThickness(float thickness) {
	m_outlineThickness = thickness;
	updateIn(); // recompute everything because the whole shape must be offset
}


////////////////////////////////////////////////////////////
float AnimeShape::getOutlineThickness() const {
	return m_outlineThickness;
}


////////////////////////////////////////////////////////////
sf::FloatRect AnimeShape::getLocalBounds() const {
	return m_bounds;
}


////////////////////////////////////////////////////////////
sf::FloatRect AnimeShape::getGlobalBounds() const {
	return getTransform().transformRect(getLocalBounds());
}

////////////////////////////////////////////////////////////
AnimeShape::AnimeShape() :
	m_texture(NULL),
	m_textureRect(),
	m_fillColor(255, 255, 255),
	m_outlineColor(255, 255, 255),
	m_outlineThickness(0),
	m_vertices(sf::TriangleFan),
	m_outlineVertices(sf::TriangleStrip),
	m_insideBounds(),
	m_bounds() {}


////////////////////////////////////////////////////////////
void AnimeShape::updateIn() {
	// Color
	updateFillColors();

	// Texture coordinates
	updateTexCoords();

	// Outline
	updateOutline();
}


////////////////////////////////////////////////////////////
void AnimeShape::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();

	// Render the inside
	states.texture = m_texture;
	target.draw(m_vertices, states);

	// Render the outline
	if (m_outlineThickness != 0) {
		states.texture = NULL;
		target.draw(m_outlineVertices, states);
	}
}


////////////////////////////////////////////////////////////
void AnimeShape::updateFillColors() {
	for (std::size_t i = 0; i < m_vertices.getVertexCount(); ++i)
		m_vertices[i].color = m_fillColor;
}


////////////////////////////////////////////////////////////
void AnimeShape::updateTexCoords() {
	for (std::size_t i = 0; i < m_vertices.getVertexCount(); ++i) {
		float xratio = m_insideBounds.width > 0 ? (m_vertices[i].position.x - m_insideBounds.left) / m_insideBounds.width : 0;
		float yratio = m_insideBounds.height > 0 ? (m_vertices[i].position.y - m_insideBounds.top) / m_insideBounds.height : 0;
		m_vertices[i].texCoords.x = m_textureRect.left + m_textureRect.width * xratio;
		m_vertices[i].texCoords.y = m_textureRect.top + m_textureRect.height * yratio;
	}
}


////////////////////////////////////////////////////////////
void AnimeShape::updateOutline() {
	// Return if there is no outline
	if (m_outlineThickness == 0.f) {
		m_outlineVertices.clear();
		m_bounds = m_insideBounds;
		return;
	}

	std::size_t count = m_vertices.getVertexCount() - 2;
	m_outlineVertices.resize((count + 1) * 2);

	for (std::size_t i = 0; i < count; ++i) {
		std::size_t index = i + 1;

		// Get the two segments shared by the current point
		sf::Vector2f p0 = (i == 0) ? m_vertices[count].position : m_vertices[index - 1].position;
		sf::Vector2f p1 = m_vertices[index].position;
		sf::Vector2f p2 = m_vertices[index + 1].position;

		// Compute their normal
		sf::Vector2f n1 = computeNormal(p0, p1);
		sf::Vector2f n2 = computeNormal(p1, p2);

		// Make sure that the normals point towards the outside of the shape
		// (this depends on the order in which the points were defined)
		if (dotProduct(n1, m_vertices[0].position - p1) > 0)
			n1 = -n1;
		if (dotProduct(n2, m_vertices[0].position - p1) > 0)
			n2 = -n2;

		// Combine them to get the extrusion direction
		float factor = 1.f + (n1.x * n2.x + n1.y * n2.y);
		sf::Vector2f normal = (n1 + n2) / factor;

		// Update the outline points
		m_outlineVertices[i * 2 + 0].position = p1;
		m_outlineVertices[i * 2 + 1].position = p1 + normal * m_outlineThickness;
	}

	// Duplicate the first point at the end, to close the outline
	m_outlineVertices[count * 2 + 0].position = m_outlineVertices[0].position;
	m_outlineVertices[count * 2 + 1].position = m_outlineVertices[1].position;

	// Update outline colors
	updateOutlineColors();

	// Update the shape's bounds
	m_bounds = m_outlineVertices.getBounds();
}


////////////////////////////////////////////////////////////
void AnimeShape::updateOutlineColors() {
	for (std::size_t i = 0; i < m_outlineVertices.getVertexCount(); ++i)
		m_outlineVertices[i].color = m_outlineColor;
}

} //namespace Animate

} //namespace Object
