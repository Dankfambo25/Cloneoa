#pragma once
#include "wesley.h"

const ushort VISVER = 0, magic = 0xAC1E;

struct OBJDATA {
	byte ID, PROPS[3];
	XY16 POS;
};

OBJDATA OBJECT__CTOR(byte in[]) {
	OBJDATA myobj;
	myobj.ID = in[0];
	// should (for) be used when its this short of ops
	myobj.PROPS[0] = in[1];
	myobj.PROPS[1] = in[2];
	myobj.PROPS[2] = in[3];
	myobj.POS.x = (in[4] * 256) + in[5];
	myobj.POS.y = (in[6] * 256) + in[7];
	return myobj;
}

struct PLAYERINFO {
	XY16 pos;
	byte ctrl//; //fast8
		, keys[8];
	//byte*keys;
	number lives, gems, stars, health; //fast32
	float speed;
};

struct VISION {
	ushort magicno, visver;//, settings;
	byte settings[2];
	XY16 start, size;
	plus objcount;
	byte*tiles;
	OBJDATA*objects;
};

#define LEVEL VISION

byte VISION__TEST[] =
{ 0xAC, 0x1E, 0, 0, 0, 0x10, 0, 0, 0, 3, 0, 3, 0, 2, 0, 2,
	0, 0, 0, 0, 0x01, 0x50, 0x01, 0x10,
	0, 0, 0, 0, 0x01, 0x50, 0x01, 0x10,
	0, 0, 0, 0, 0x01, 0x50, 0x01, 0x10,
	0, 0, 0, 0, 0x01, 0x50, 0x01, 0x10,
	0, 0, 0, 0, 0x01, 0x50, 0x01, 0x10 };

VISION VISION__CTOR(byte in[]) {
	//plus vissz = sizeof(in); NOPE >:(
	plus vissz = 96;
	if (vissz >= 0x02) {
		VISION myvis;

		myvis.magicno = (in[0] * 256) + in[1];
		myvis.visver = (in[2] * 256) + in[3];
		myvis.settings[0] = in[4];
		myvis.settings[1] = in[5];

		myvis.size.x = (in[8] * 256) + in[9];
		myvis.size.y = (in[0xA] * 256) + in[0xB];

		myvis.start.x = (in[0xC] * 256) + in[0xD];
		myvis.start.y = (in[0xE] * 256) + in[0xF];

		plus tilsz = myvis.size.x * myvis.size.y;

		myvis.tiles = new byte[tilsz];

		for (plus i = 0; i < tilsz; i++)
		{
			myvis.tiles[i] = (in[16 + i]);
		}

		myvis.objcount = (vissz - tilsz) / 8;

		myvis.objects = new OBJDATA[myvis.objcount];

		// new jersey kid cant do **** with c++

		int temppad = (tilsz + (16 - (tilsz % 16)));
		if ((tilsz % 16) == 0)
			temppad -= 16;

		for (plus i = tilsz + 16; i < vissz; i += 8)
		{
			myvis.objects[i] = OBJECT__CTOR(
				new byte[
					in[i], in[i + 1], in[i + 2], in[i + 3],
					in[i + 4], in[i + 5], in[i + 6], in[i + 7]
				]);
		}

		// align with x + (16 - (x % 16)) and then 0 if just fits

		return myvis;
	}
	else
		// throw std::runtime_error("Invalid size");
	{
		VISION failvis;
		failvis.tiles = new byte['E','R','R','O','R','!','!','!'];
		return failvis;
	}
}

enum objtype {
	item,
	entity,
	door,
	sign
};

enum item {
	gem,
	diamond,
	heart,
	star,
	life
};

enum entity {
	moo,
	mfly,
	box,
	ball
};

enum door {
	goal,
	locked0,
	locked1,
	warp
};

enum tile {
	air,
	ground,
	noclip,
	damage,
	turbulance
};

