/*
 * TrackBar.h
 *
 *  Created on: 2016-10-09
 *      Author: dannyphantom
 */

#ifndef TRACKBAR_H_
#define TRACKBAR_H_

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GL/glew.h"
#include "SOIL.h"

/**
 * Class that represents a trackbar. Each trackbar has one value
 * that can be changed by dragging the slider.
 * It is a rectangle with another rectangle inside it.
 * The inner rectangle represents the range by which the
 * value can be changed. Finally, a small rectangle is drawn on top of
 * everything, and represents the actual value. Value is always
 * clamped to the allowed values.
 * It also holds a texture that is shown to the left side of the trackbar
 * itself.
 */
class TrackBar {

private:
	float minValue, maxValue, currentValue;		//range of possible values and the current value
	float width, height;						//size of the outer box
	glm::vec3 color;							//color of the outer box
	glm::vec2 center;							//center of the box

	GLuint program;								//shader program for the trackbar
	GLuint VAO;									//vao
	GLuint texId;								//texture id

	bool mouseStuck = false;					//indicates whether the value of this trackbar is being changed ATM

public:
	TrackBar(float minValue, float maxValue, float width, float height, GLuint program, char* imagePath);
	virtual ~TrackBar();

	void setCurrentValue(float value);
	void setColor(glm::vec3 color);
	void setCenter(glm::vec2 center);
	void render();

	bool mousePressed(float x, float y);
	void mouseReleased();
	void setNewMousePosition(float x, float y);

	float getCurrentValue();
	bool getMouseStuck();

};

#endif /* TRACKBAR_H_ */
