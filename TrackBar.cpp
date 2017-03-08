/*
 * TrackBar.cpp
 *
 *  Created on: 2016-10-09
 *      Author: dannyphantom
 */

#include "TrackBar.h"
#include <stdio.h>

TrackBar::TrackBar(float minValue, float maxValue, float width, float height, GLuint program, char *imagePath) :
		minValue(minValue), maxValue(maxValue), width(width), height(height), currentValue(minValue),
		program(program), color(1.0, 0.0, 0.0), center(0.0, 0.0){

	//create a rectangle with 4 vertices
	float vertices[] = {
				-1, 1,		//BL
				-1, -1, 	//TL
				1, 1,		//BR
				1, -1,		//TR
		};

	// and create the uv's for the vertices
	float uvs[] = {
			0, 0,		//BL
			0, 1, 	//TL
			1, 0,		//BR
			1, 1,		//TR
	};

	GLuint VBO_vertices, VBO_uvs;

	//passing model attributes to the GPU
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//vertices
	glGenBuffers(1, &VBO_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	//uv's
	glGenBuffers(1, &VBO_uvs);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_uvs);
	glBufferData(GL_ARRAY_BUFFER, sizeof (uvs), uvs, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	//load the texture
	texId = SOIL_load_OGL_texture
			(
				imagePath,
				SOIL_LOAD_AUTO,
				SOIL_CREATE_NEW_ID,
				SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
			);

	if (texId == 0) printf( "SOIL loading error: '%s' %s\n ", SOIL_last_result(), imagePath );

	//set texture parameters
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
}

TrackBar::~TrackBar() {

}

void TrackBar::setCurrentValue(float value) {
	this->currentValue = value;
}

void TrackBar::setColor(glm::vec3 color) {
	this->color = color;
}

void TrackBar::setCenter(glm::vec2 center) {
	this->center = center;
}

void TrackBar::render() {
	glUseProgram(program);

	glBindVertexArray(VAO);

	//we indicate that the next 3 drawings don't use a texture
	glUniform1i(glGetUniformLocation(program, "textureValid"), false);

	//draw the outer box
	glm::mat4 identity(1.0f);
	glm::mat4 translation = glm::translate(identity, glm::vec3(center.x, center.y, 0.1));
	glm::mat4 scale = glm::scale(identity, glm::vec3(width, height, 1.0));
	glm::mat4 modelMatrix = translation * scale;

	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

	glUniform3fv(glGetUniformLocation(program, "barColor"), 1, glm::value_ptr(color) );

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);




	//draw the inner box
	translation = glm::translate(identity, glm::vec3(center.x, center.y, 0.0));
	scale = glm::scale(identity, glm::vec3(width * 0.9, height * 0.1, 1.0));
	modelMatrix = translation * scale;

	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

	glm::vec3 colorInner(1.0, 1.0, 1.0);
	glUniform3fv(glGetUniformLocation(program, "barColor"), 1, glm::value_ptr(colorInner) );

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);




	//draw the slider
	translation = glm::translate(identity, glm::vec3(center.x - width * 0.9 + width * 1.8 * (currentValue - minValue) / (maxValue - minValue), center.y, -0.1));
	scale = glm::scale(identity, glm::vec3(height * 0.5, height * 0.5, 1.0));
	modelMatrix = translation * scale;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

	glUniform3fv(glGetUniformLocation(program, "barColor"), 1, glm::value_ptr(colorInner) );

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);



	//now enable the texture and draw it to the left of the trackbar
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	translation = glm::translate(identity, glm::vec3(center.x - width * 1.07f, center.y, 0.0f));
	scale = glm::scale(identity, glm::vec3(height, height, 1.0));
	modelMatrix = translation * scale;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

	glUniform1i(glGetUniformLocation(program, "textureValid"), true);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texId);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	glBindVertexArray(0);
}

float TrackBar::getCurrentValue() {
	return this->currentValue;
}

/**
 * This function identifies if the mouse click happened on the slider.
 * If so, it is "stuck" to the current trackbar and all the mouse movements
 * will now affect the value of the trackbar
 */
bool TrackBar::mousePressed(float x, float y) {
	//find the position of the slider
	glm::vec3 circlePos = glm::vec3(center.x - width * 0.9 + width * 1.8 * (currentValue - minValue) / (maxValue - minValue), center.y, -0.1);
	float circleSize = height;

	//if the mouse click happened "on" the slider
	if (x <= circlePos.x + circleSize
			&& x >= circlePos.x - circleSize
			&& -y <= circlePos.y + circleSize
			&& -y >= circlePos.y - circleSize) {
		//then we start tracking all mouse changes
		mouseStuck = true;
		return true;
	}

	return false;
}

void TrackBar::mouseReleased() {
	mouseStuck = false;
}

bool TrackBar::getMouseStuck() {
	return mouseStuck;
}

/**
 * When we are "stuck" to the trackbar, then all the mouse movements
 * have to be translated to the new current value for this trackbar
 */
void TrackBar::setNewMousePosition(float x, float y) {
	//we care only about x, not y

	//get the current value from -inf to +inf
	float currentValue = (x - (center.x - width * 0.9)) / (width * 1.8);

	//clamp it to 0 to 1
	if (currentValue < 0) currentValue = 0.0;
	if (currentValue > 1) currentValue = 1.0;

	//now put it within our allowed boundaries
	currentValue = minValue + (maxValue - minValue) * currentValue;

	//set the instance variable to this one
	this->currentValue = currentValue;
}
