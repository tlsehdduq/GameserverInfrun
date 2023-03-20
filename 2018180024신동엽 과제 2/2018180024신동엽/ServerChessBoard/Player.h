#pragma once
#include"stdafx.h"

class Player
{
public:
	Player();
	~Player();

	Player(int _x, int _y, int _w, int _h);

	void Render(HDC hdc, HBITMAP hBitmap, int _x, int _y, int _width, int _height);
	void Move(int dx, int dy);
	void Update();
	int getPosX() { return posX; }
	int getPosY() { return posY; }
	int getWidth() { return width; }
	int getHeight() { return height; }
	void setPosX(int x) { posX = x; }
	void setPosY(int y) { posY = y; }

private:
	int posX = 0;
	int posY = 0;
	int width;
	int height;

};

