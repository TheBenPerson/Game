#include <GL/gl.h>
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include "Button/button.hpp"
#include "menu.hpp"
#include "Rendering/rendering.hpp"
#include "Util/NodeList/nodelist.hpp"
#include "XClient/xclient.hpp"

bool Menu::active = true;
GLuint Menu::background;
bool Menu::inGame = false;

void settingss() {

	Menu::panel = Menu::settings;

}

void back() {

	Menu::panel = Menu::mainMenu;

}

void quit() {


}

Button* Menu::mainMenu[] = {

	new Button({0.0f, 2.5f}, 10.0f, "Start", NULL),
	new Button({0.0f, 0.0f}, 10.0f, "Settings", settingss),
	new Button({0.0f, -2.5f}, 10.0f, "Quit", NULL), NULL

};
Button* Menu::settings[] = {

	new Button({0.0f, -3.0f}, 10.0f, "Back", back),
	new Button({0.0f, -3.0f}, 10.0f, "Back", back),
	new Button({0.0f, -3.0f}, 10.0f, "Back", back), NULL

};
Button** Menu::panel = mainMenu;

void Menu::cleanup() {

	glDeleteTextures(1, &background);

	for (size_t i = 0; mainMenu[i]; i++)
		delete mainMenu[i];

	Button::cleanup();

}

void Menu::draw() {

	static float i = 0.0f;
	i += 0.005f;

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();

	glTranslatef(sinf(i), cosf(i), 0.0f);
	glRotatef(i * 10, 0.0f, 0.0f, 1.0f);

	glBindTexture(GL_TEXTURE_2D, background);
	glBegin(GL_QUADS);

		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(XClient::aspect * -10, 10.0f);
		glTexCoord2f(XClient::aspect, 1.0f);
		glVertex2f(XClient::aspect * 10, 10.0f);
		glTexCoord2f(XClient::aspect, 0.0f);
		glVertex2f(XClient::aspect * 10, -10.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(XClient::aspect * -10, -10.0f);

	glEnd();

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	for (size_t i = 0; panel[i]; i++)
		panel[i]->draw();

}

void Menu::init() {

	Button::init();
	background = Rendering::loadTexture("res/background.png");

}
