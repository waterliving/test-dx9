/**
*** Autor: Jan "kijanek6" Koœciñski
*** mail: kijanek6@wp.pl || GG: 5487004
*** 30.11.2009 dla Kijedi Studio
***
*** Ten utwór objêty jest licencj¹ Creative Commons
*** Uznanie autorstwa-U¿ycie niekomercyjne-Na tych
*** samych warunkach 3.0 Polska. Aby zobaczyæ kopiê
*** niniejszej licencji przejdŸ na stronê
*** http://creativecommons.org/licenses/by-nc-sa/3.0/pl/
*** lub napisz do Creative Commons, 171 Second Street,
*** Suite 300, San Francisco, California 94105, USA.
**/
#include "CLight.h"

#ifndef RAD
#define Distance(x1,x2,y1,y2) sqrt((x2-x1)^2+(y2-y1)^2)
#define Between(x, y, z) (x > y && x < z)
#define shPos(x, y) ((y)*800+(x))
#define RAD 57.295780
#define RadToDeg(x) (x*RAD)
#define DegToRad(x) (x/RAD)
#define M_PI_X2 6.283185
void RenderPoint(float x, float y, DWORD col);
#endif

extern int iPPO;

//kontruktor dla klasy swiatla
CLight::CLight(float x, float y, float nAngle, float nPower, float nDir)
{
    pos.x = x;
    pos.y = y;
    angle = 360/nAngle;
    power = nPower;
    dir = DegToRad(nDir);
    r = 0;
    g = 0;
    b = 0;
}

//destruktor
CLight::~CLight()
{

}

//manipulujemy maska. funkcja jest po to, aby nie wyjsc poza rozmiar tekstury
//gdybysmy probowali manipulowac na 801x601 to by sie program zwiesil
void CLight::Manipulate(int posx, int posy, DWORD col)
{
    if( Between(posx, 0, 800) && Between(posy, 0, 600) ){
     iPPO++;
     mem[shPos(posx, posy)] = col;
    }
}

//pobieramy piksel z maski. podobnie jak wyzej
DWORD CLight::GetPixel(int posx, int posy)
{
    if( Between(posx, 0, 800) && Between(posy, 0, 600) )
     return mem[shPos(posx, posy)];
}

//wycinamy nasze swiatlo na masce
void CLight::Render(DWORD * data)
{
    mem = data;
    float alphastep = 255 / power;
    float step = power;
     for( float ang=-3.141593+dir+DegToRad(89.535-(360/(angle)/2)); ang<=-3.141593+dir+M_PI_X2/angle+DegToRad(89.535-(360/(angle)/2)); ang+=(M_PI_X2/(step*15)) ){
      bool alive=1;
      float cosinus=cos(ang), sinus=sin(ang);
      for( int radius=0;radius<power;radius++ ){
       int posx = radius*cosinus + pos.x;
       int posy = radius*sinus + pos.y;
       if( GetPixel(posx, posy) == 0xFFFFFFFF || !alive ){
        alive = 0;
        if( radius*(alphastep*2) >= 255 || GetPixel(posx, posy) == 0xFFFF00FF ) break;
        Manipulate(posx, posy, ARGB(radius*(alphastep*2), 0, 0, 0));
        iPPO++;
       }else{
        iPPO++;
        Manipulate(posx, posy, ARGB(radius*alphastep, r, g, b));
       }
      }
     }

    RenderPoint(pos.x, pos.y, 0xFF0000FF);
}

//ustawiamy kolor naszemu swiatlu
void CLight::SetColor(int nr, int ng, int nb)
{
    r = nr;
    g = ng;
    b = nb;
}

//ustawiamy pozycje
void CLight::SetPos(hgeVector npos)
{
    pos.x = npos.x;
    pos.y = npos.y;
}

//pobieramy pozycje
hgeVector CLight::GetPos()
{
    return pos;
}

//ustawiamy sile (promien)
void CLight::SetPower(float nPower)
{
    power = nPower;
}

//pobieramy sile (promien)
float CLight::GetPower()
{
    return power;
}

//pobieramy kat swiatla
float CLight::GetAngle()
{
    return 360/angle;
}

//ustawiamy kat swiatla
void CLight::SetAngle(float nAngle)
{
    if( nAngle > 360 ) nAngle = 360;
    if( nAngle < 0 ) nAngle = 1;
    angle = 360/nAngle;
}

//ustawiamy kierunek swiatla
void CLight::SetDirection(float nDir)
{
    dir = DegToRad(nDir);
}

//pobieramy kierunek swiatla
float CLight::GetDirection()
{
    return RadToDeg(dir);
}
