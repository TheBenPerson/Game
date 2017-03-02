/*

Copyright (C) 2016-2017 Ben Stockett

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <GL/gl.h>
#include <stdio.h>
#include "button.hpp"
#include "Rendering/rendering.hpp"
#include "Util/Point/point.hpp"
#include "XInput/xinput.hpp"

GLuint Button::texture;

Button::Button(Point pos, float width, char *name, void (*callback)()) : name(name), callback(callback) {

	float dX = width / 2.0f;

	values[0].x = pos.x - dX;
	values[0].y = pos.y - 1.0f;

	values[1].x = pos.x + dX;
	values[1].y = pos.y + 1.0f;

	this->width = width - 2.0f;

}

void Button::cleanup() {

	glDeleteTextures(1, &texture);

}

void Button::draw() {

	glPushMatrix();

		glMatrixMode(GL_TEXTURE);
		glPushMatrix();

			glTranslatef(0.0f, -state * (1.0f / 8.0f), 0.0f);

			glBindTexture(GL_TEXTURE_2D, texture);
			glBegin(GL_QUADS);

				glTexCoord2f(0.0f, 5.0f / 8.0f);
				glVertex2f(values[0].x, values[1].y);
				glTexCoord2f(1.0f, 5.0f / 8.0f);
				glVertex2f(values[0].x + 1.0f, values[1].y);
				glTexCoord2f(1.0f, 4.0f / 8.0f);
				glVertex2f(values[0].x + 1.0f, values[0].y);
				glTexCoord2f(0.0f, 4.0f / 8.0f);
				glVertex2f(values[0].x, values[0].y);

				glTexCoord2f(0.0f, 1.0f);
				glVertex2f(values[0].x + 1.0f, values[1].y);
				glTexCoord2f(width, 1.0f);
				glVertex2f(values[1].x - 1.0f, values[1].y);
				glTexCoord2f(width, 7.0f / 8.0f);
				glVertex2f(values[1].x - 1.0f, values[0].y);
				glTexCoord2f(0.0f, 7.0f / 8.0f);
				glVertex2f(values[0].x + 1.0f, values[0].y);

				glTexCoord2f(1.0f, 5.0f / 8.0f);
				glVertex2f(values[1].x - 1.0f, values[1].y);
				glTexCoord2f(0.0f, 5.0f / 8.0f);
				glVertex2f(values[1].x, values[1].y);
				glTexCoord2f(0.0f, 4.0f / 8.0f);
				glVertex2f(values[1].x, values[0].y);
				glTexCoord2f(1.0f, 4.0f / 8.0f);
				glVertex2f(values[1].x - 1.0f, values[0].y);


			glEnd();

		glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	Rendering::drawText(name, {values[0].x + ((values[1].x - values[0].x) / 2), values[1].y + ((values[0].y - values[1].y) / 2)}, 1.0f, true);

}

void Button::init() {

	texture = Rendering::loadTexture("res/button.png");

}

void Button::tick() {

	if (((XInput::mousePos.x > values[0].x) && (XInput::mousePos.y > values[0].y)) && ((XInput::mousePos.x < values[1].x) && (XInput::mousePos.y < values[1].y))) {

		if (XInput::keys[XInput::LMOUSE]->state) state = clicked;
		else {

			if (state == clicked) callback();
			state = hover;

		}

	} else  state = normal;

}
