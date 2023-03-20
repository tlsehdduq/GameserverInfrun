#include "Player.h"

Player::Player() : posX(0), posY(0),width(0),height(0)
{
}

Player::~Player()
{
}

Player::Player(int _x, int _y, int _w, int _h)
{
	posX = _x;
	posY = _y;
	width = _w;
	height = _h;
}


void Player::Render(HDC hdc, HBITMAP hBitmap, int _x, int _y, int _width, int _height )
{
	
	 HDC hMemDC = CreateCompatibleDC(hdc);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
        StretchBlt(hdc, _x, _y, width, height, hMemDC, 0, 0, _width, _height, SRCCOPY);
        SelectObject(hMemDC, hOldBitmap);
        DeleteDC(hMemDC);

}

void Player::Move(int dx, int dy)
{
	posX += dx;
	posY += dy;

}

void Player::Update()
{
 
}



