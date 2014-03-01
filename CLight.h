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
#ifndef CLIGHT_H
#define CLIGHT_H
#include "hgevector.h"
//klasa ze swiatlem
class CLight {
    private:
     int r,g,b;
     hgeVector pos;
     float angle;
     float dir;
     float power;
     DWORD * mem;
     void Manipulate(int posx, int posy, DWORD col);
     DWORD GetPixel(int posx, int posy);
    public:
     CLight(float x, float y, float nAngle, float nPower, float nDir);
     ~CLight();
     void Render(DWORD * data);
     void SetPos(hgeVector npos);
     void SetPower(float nPower);
     float GetPower();
     float GetAngle();
     void SetAngle(float nAngle);
     void SetDirection(float nDir);
     float GetDirection();
     hgeVector GetPos();
     void SetColor(int nr, int ng, int nb);
};

#endif
