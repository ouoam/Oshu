#pragma once

namespace Object {

class Container : public sf::Drawable {
	virtual void onMouseClick(uint8_t) {};
	virtual void onMouseDown(uint8_t) {};
	virtual void onMouseUp(uint8_t) {};
	virtual void onMouseMove(uint8_t) {};
};

}