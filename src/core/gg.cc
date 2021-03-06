#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "gg_window.hh"
#include "gg.hh"


namespace gg 
{
namespace
{
/* run
Whether the app is running or not. */

bool running = false;

/* windows
Array of pointers to windows. */

std::vector<Window*> windows;

/* font
Pointer to a TTF resource. */

TTF_Font* font;	


/* cleanup()
Cleans up the Window vector and quits the SDL app. */

void cleanup()
{
	for (Window* w : windows)
		delete w;
	windows.clear();
	SDL_Quit();
	TTF_Quit();
}

} // {anonymous}


/* -------------------------------------------------------------------------- */


FontInfo fontInfo;


int init()
{
	running = false;
	font    = nullptr;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("[gg::init] unable to initialize: %s\n", SDL_GetError());
		return 0;
	}
	if (TTF_Init() == -1)
	{
		printf("[gg::init] unable to initialize text rendering: %s\n", TTF_GetError());
		return 0;
	}
	running = true;
	return 1;
}


/* -------------------------------------------------------------------------- */


int loadFont(const char* f, int s)
{
	font = TTF_OpenFontIndex(f, s, 0);
	if (!font)
	{
		printf("[ggWidget::setFont] unable to load font: %s\n", TTF_GetError());
		return 0;
	}
	fontInfo.height = TTF_FontHeight(font);
	return 1;
}


/* -------------------------------------------------------------------------- */


FontInfo& getFontInfo()
{
	return fontInfo;
}


/* -------------------------------------------------------------------------- */


void drawFont(SDL_Renderer* ren, SDL_Rect rc, const char* t)
{
	SDL_Color color      = { 255, 255, 255 };
	SDL_Surface* surf    = TTF_RenderText_Solid(font, t, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, surf);
	SDL_FreeSurface(surf);

	int tw, th;
	TTF_SizeText(font, t, &tw, &th);

	if (tw > rc.w)
		printf("[drawFont] warning: string overflow (%d px)\n", tw - rc.w);

	rc.x = rc.x + (rc.w / 2) - (tw / 2);
	rc.y = rc.y + (rc.h / 2) - (th / 2);
	rc.w = tw;
	rc.h = th;

	SDL_RenderCopy(ren, texture, nullptr, &rc);
}


/* -------------------------------------------------------------------------- */


void freeFont()
{
	TTF_CloseFont(font);
}


/* -------------------------------------------------------------------------- */


int run()
{
	SDL_Event e;
	while (running)
	{
		SDL_WaitEvent(&e);
		if (e.type == SDL_QUIT)
		{
			puts("[gg::run] SDL_QUIT event, stop event loop");
			running = false;
		}
		else
		{
			for (Window* w : windows)
				w->handle(e);
		}
	}
	cleanup();
	return 1;
}


/* -------------------------------------------------------------------------- */


void add(Window* w)
{
	windows.push_back(w);
	printf("[gg::add] add window %p, id=%d, windows size %zd\n",
		(void*)w, w->id, windows.size());
}


/* -------------------------------------------------------------------------- */


void quit()
{
	SDL_Event e;
  e.type = SDL_QUIT;
	SDL_PushEvent(&e);
}


} // gg::