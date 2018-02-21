#include <GL/gl.h>
#include <math.h>
#include <stdint.h>

#include "console.hh"
#include "entity.hh"
#include "gfx.hh"
#include "net.hh"
#include "packet.hh"
#include "world.hh"

static GLuint texFire;

static void initGL();
static void cleanupGL();
static bool tickNet(Packet *packet);
static void draw();

extern "C" {

	bool init() {

		GFX::call(&initGL);

		Net::listeners.add((void*) &tickNet);
		World::listeners.add((void*) &draw);

		Net::send(P_GENT);

		cputs(GREEN, "Loaded module: 'entity.so'");
		return true;

	}

	void cleanup() {

		while (Entity::entities.len) {

			Entity *entity = (Entity*) Entity::entities.get(0);
			delete entity;

		}

		World::listeners.rem((void*) &draw);
		Net::listeners.rem((void*) &tickNet);

		GFX::call(&cleanupGL);

		cputs(YELLOW, "Unloaded module: 'entity.so'");

	}

}

void initGL() {

	texFire = GFX::loadTexture("fire.png");

}

void cleanupGL() {

	glDeleteTextures(1, &texFire);

}

bool tickNet(Packet *packet) {

	switch (packet->id) {

		case P_UENT: {

			Entity *entity = Entity::get(*((uint16_t*) packet->data));
			entity->update(packet->data + 2);

		} break;

		case P_DENT: {

			Entity *entity = Entity::get(*((uint16_t*) packet->data));
			delete entity;

		} break;

		default: return false;

	}

	return true;

}

void draw() {

	for (unsigned int i = 0; i < Entity::entities.len; i++) {

		Entity *entity = (Entity*) Entity::entities.get(i);
		entity->draw();

	}

}

NodeList Entity::entities;

Entity* Entity::get(unsigned int id) {

	for (unsigned int i = 0; i < entities.len; i++) {

		Entity *entity = (Entity*) entities.get(i);
		if (entity->id == id) return entity;

	}

	return NULL;

}

Entity::Entity(void *info) {

	struct Data {

		uint16_t id;

		uint8_t pad[];

	} __attribute__((packed)) *data = (Data*) info;

	id = data->id;
	update(info + 2);

	entities.add((void*) this);

}

Entity::~Entity() {

	entities.rem((void*) this);

}

void Entity::draw() {

	if (onfire) {

		glPushMatrix();
		glTranslatef(pos.x, pos.y, 0);
		glRotatef(-(World::rot * 360) / (M_PI * 2), 0, 0, 1);
		glScalef(dim.x / 2, dim.y / 2, 1);

		glMatrixMode(GL_TEXTURE);
		glPushMatrix();
		glTranslatef((GFX::frame / 5) / 10.0f, 0, 0);

		glBindTexture(GL_TEXTURE_2D, texFire);
		glBegin(GL_QUADS);

		glTexCoord2f(0, 1);
		glVertex2f(-1, 1);
		glTexCoord2f(.1f, 1);
		glVertex2f(1, 1);
		glTexCoord2f(.1f, 0);
		glVertex2f(1, -1);
		glTexCoord2f(0, 0);
		glVertex2f(-1, -1);

		glEnd();

		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

	}

}

void Entity::update(void *info) {

	struct Data {

		__attribute__((packed)) Point dim;
		__attribute__((packed)) Point pos;
		__attribute__((packed)) Point vel;

		float rot;
		bool onfire;

		uint8_t pad[];

	} __attribute__((packed)) *data = (Data*) info;

	dim = data->dim;
	pos = data->pos;
	vel = data->vel;

	rot = data->rot;
	onfire = data->onfire;

}
