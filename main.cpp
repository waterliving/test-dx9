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
#include "hge.h"
#include "hgevector.h"
#include "hgesprite.h"
#include <stdio.h>

//makra do obliczania odleglosci miedzy punktami i takie tam
#define Distance(x1,x2,y1,y2) sqrt((x2-x1)^2+(y2-y1)^2)
#define Between(x, y, z) (x > y && x < z)
#define shPos(x, y) ((y)*800+(x))
#define RAD 57.295780
#define RadToDeg(x) (x*RAD)
#define DegToRad(x) (x/RAD)
#define M_PI_X2 6.283185

#include "CLight.h"

//licznik ile wykonalismy operacji per pixel
int iPPO;
//swiatla
CLight * Light;
CLight * Light2;
bool bLight2;
//tekstury
HTEXTURE texture;
HTEXTURE shadow;
int map[9][7] = {1,1,1,1,1,1,1, //plansza
                 1,0,0,1,0,0,0,
                 1,0,0,0,0,1,0,
                 1,1,1,1,1,1,0,
                 1,0,0,1,1,1,0,
                 0,0,0,1,0,1,0,
                 0,0,0,0,0,0,0,
                 0,1,0,1,0,1,0,
                 0,0,0,1,1,1,1};
//grafiki: dwa klocki i maska
hgeSprite * boxtexture1, * boxtexture2, * shadowsprite;
//uchwyt do silnika
HGE *hge = 0;
//baza cial
hgeQuad body[63];
//baza pozycji cial
hgeVector bodypos[63];
//ile mamy cial dajacych cien?
int bodies;
//funkcja do rysowania punktu
void RenderPoint(float x, float y, DWORD col);
//przeciazona funkcja do rysowania punktu (tutaj z wektora)
void RenderPoint(hgeVector point, DWORD col);

bool FrameFunc()
{
    if( hge->Input_KeyDown(HGEK_L) )
     bLight2 = !bLight2;
    //pobieramy pozycje myszki do *mx i *my
    float *mx = new float, *my = new float;
    hge->Input_GetMousePos(mx, my);
    //zmieniamy sile/kat swiatla za pomoca strzalek
    if( hge->Input_GetKeyState(HGEK_UP) )
     Light->SetPower(Light->GetPower()+5);
    else if( hge->Input_GetKeyState(HGEK_DOWN) )
     Light->SetPower(Light->GetPower()-5);
    if( hge->Input_GetKeyState(HGEK_LEFT) )
     Light->SetAngle(Light->GetAngle()-5);
    else if( hge->Input_GetKeyState(HGEK_RIGHT) )
     Light->SetAngle(Light->GetAngle()+5);
    //poruszamy swiatlem
     hgeVector pos = Light->GetPos();
     hgeVector move;
     move.x = move.y = 0;
     if( hge->Input_GetKeyState(HGEK_A) )
      move.x = -50;
     else if( hge->Input_GetKeyState(HGEK_D) )
      move.x = 50;
     if( hge->Input_GetKeyState(HGEK_W) )
      move.y = -100;
     else if( hge->Input_GetKeyState(HGEK_S) )
      move.y = +100;
     if( move.x != 0 || move.y != 0 ){
      move.Rotate(DegToRad(Light->GetDirection()));
      pos.x += move.x*hge->Timer_GetDelta();
       pos.y += move.y*hge->Timer_GetDelta();
      Light->SetPos(pos);
     }
     //ustawiamy kat swiatla
     Light->SetDirection(Light->GetDirection()-(400-*mx));
    //ustawiamy myszke na srodek ekranu
    hge->Input_SetMousePos(400, 300);
    //uaktualniamy belke
    char title[128];
    sprintf(title, "FPS: %d, per pixel operations: %d", hge->Timer_GetFPS(), iPPO);
	hge->System_SetState(HGE_TITLE, title);
	//konczymy program przy wcisnieciu esc
    if( hge->Input_KeyDown(HGEK_ESCAPE) )
     return 1;
    return 0;
}

bool RenderFunc()
{
    //resetujemy ilosc wykonanych operacji per pixel
    iPPO = 0;
    hge->Gfx_BeginScene(); //zaczynamy rysowac scene
    hge->Gfx_Clear(0xFF747474); //czyscimy ekran szarym kolorem

    //rysujemy plansze
    for(int y=0;y<7;y++)
     for(int x=0;x<9;x++)
      if( map[x][y] )
       boxtexture1->Render(x*90, y*90);
      else
       boxtexture2->Render(x*90, y*90);

    //pobieramy do pamieci teksture naszej maski
    DWORD * shadowmask = hge->Texture_Lock(shadow, false, 800, 600);
    //wypelniamy czarnym kolorem
    for(DWORD i=0;i<480000;i++){
     shadowmask[i] = 0xFF000000; iPPO++; }

    //iterujemy przez ciala i rysujemy je na naszej masce
    for(int i=0;i<bodies;i++)
     for(int ix=0;ix<90;ix++)
      for(int iy=0;iy<90;iy++)
       if( Between(int(bodypos[i].x)*90+ix, 0, 800) && Between(int(bodypos[i].y)*90+iy, 0, 600) ){
        if( ix==0 || ix==89 || iy==0 || iy==89 )
         shadowmask[shPos(int(bodypos[i].x)*90+ix, int(bodypos[i].y)*90+iy)] = 0xFFFF00FF;
        else
         shadowmask[shPos(int(bodypos[i].x)*90+ix, int(bodypos[i].y)*90+iy)] = 0xFFFFFFFF;
        iPPO++;
       }

    //wycinamy nasze swiatla na masce
    Light->Render(shadowmask);
    if( bLight2 )
     Light2->Render(shadowmask);

    //iterujemy ponownie przez ciala i usuwamy pozostalosci po nich z maski
    for(int i=0;i<bodies;i++)
     //if( i == 14 )
     for(int ix=0;ix<90;ix++)
      for(int iy=0;iy<90;iy++)
       if( Between(int(bodypos[i].x)*90+ix, 0, 800) && Between(int(bodypos[i].y)*90+iy, 0, 600) )
        if( shadowmask[shPos(int(bodypos[i].x)*90+ix, int(bodypos[i].y)*90+iy)] == 0xFFFFFFFF
         || shadowmask[shPos(int(bodypos[i].x)*90+ix, int(bodypos[i].y)*90+iy)] == 0xFFFF00FF ){
         shadowmask[shPos(int(bodypos[i].x)*90+ix, int(bodypos[i].y)*90+iy)] = 0xFF000000;
         iPPO++;
        }
    //odblokowujemy teksture
    hge->Texture_Unlock(shadow);
    //rysujemy maske
    shadowsprite->Render(0, 0);

    hge->Gfx_EndScene(); //konczymy rysowanie sceny
    return 0;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    //iterujemy przez planszy i generujemy ciala
    for(int y=0;y<7;y++){
     for(int x=0;x<9;x++)
      if( map[x][y] ){
       bodypos[bodies].x = x;
       bodypos[bodies].y = y;
       body[bodies].tex = 0;
       body[bodies].blend = BLEND_DEFAULT;
       body[bodies].v[0].col = body[bodies].v[1].col = body[bodies].v[2].col = body[bodies].v[3].col = 0xFF00ffff;
       body[bodies].v[0].x = x*90; body[bodies].v[0].y = y*90;
       body[bodies].v[1].x = x*90+90; body[bodies].v[1].y = y*90;
       body[bodies].v[2].x = x*90+90; body[bodies].v[2].y = y*90+90;
       body[bodies].v[3].x = x*90; body[bodies].v[3].y = y*90+90;
       bodies++;
      }
    }

    //tworzymy nasze swiatla
    Light = new CLight(100, 100, 90, 250, 180);
    Light2 = new CLight(750, 50, 360, 150, 0);
    Light2->SetColor(255,0,0);
    bLight2=0;

    //tworzymy okno hge
	hge = hgeCreate(HGE_VERSION);

    //konfigurujemy
	hge->System_SetState(HGE_SHOWSPLASH, 0);
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_TITLE, "2D Dynamic Shadows [Kijedi Studio]");
	hge->System_SetState(HGE_WINDOWED, true);
	hge->System_SetState(HGE_USESOUND, false);
	hge->System_SetState(HGE_LOGFILE, "log.txt");

	if(hge->System_Initiate()){
     shadow = hge->Texture_Create(800, 600); //tworzymy teksture maski
     shadowsprite = new hgeSprite(shadow, 0, 0, 800, 600); //tworzymy sprajt dla maski
     shadowsprite->SetBlendMode(BLEND_COLORMUL|BLEND_ALPHABLEND); //ustawiamy tryb mieszania
     texture = hge->Texture_Load("obrazki.png"); //ladujemy teksture
     boxtexture1 = new hgeSprite(texture, 0, 0, 90, 90); //tworzymy sprajty planszy
     boxtexture2 = new hgeSprite(texture, 90, 0, 90, 90); //tworzymy sprajty planszy
     hge->System_Start(); //wlaczamy hge
	}else
     MessageBox(NULL, hge->System_GetErrorMessage(), "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL); //wyrzucamy blad

    //konczymy
	hge->System_Shutdown();
	hge->Release();
	return 0;
}

//rysujemy kwadrat 10x10 w miejscu wskazanym przez x i y (sluzy do rysowania pozycji swiatla)
void RenderPoint(float x, float y, DWORD col)
{
    hge->Gfx_RenderLine(x-5, y-5, x+5, y-5, col);
    hge->Gfx_RenderLine(x-5, y-5, x-5, y+5, col);
    hge->Gfx_RenderLine(x+5, y+5, x+5, y-5, col);
    hge->Gfx_RenderLine(x+5, y+5, x-5, y+5, col);
}

//-||-
void RenderPoint(hgeVector point, DWORD col)
{
    RenderPoint(point.x, point.y, col);
}
