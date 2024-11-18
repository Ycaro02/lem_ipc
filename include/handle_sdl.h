#ifndef HANDLE_SDL_H
#define HANDLE_SDL_H

/* Basic SDL include */
#include "../rsc/lib/install/include/SDL2/SDL.h"
#include "../rsc/lib/install/include/SDL2/SDL_video.h"
#include "../rsc/lib/install/include/SDL2/SDL_render.h"
/* TTF include */
#include "../rsc/lib/install/include/SDL2/SDL_ttf.h"

#include "../libft/basic_define.h"
#include "../libft/libft.h"
// #include "../include/display.h"

#include <stdio.h>

/* Macro to convert RGBA to UINT32 and UINT32 to RGBA */
#define		RGBA_TO_UINT32(r, g, b, a)		((u32)((a << 24) | (r << 16) | (g << 8) | b))

#define		UINT32_TO_RGBA(color, r, g, b, a)	\
	do { \
		r = (color >> 16) & 0xFF;				\
		g = (color >> 8) & 0xFF;				\
		b = color & 0xFF;						\
		a = (color >> 24) & 0xFF;				\
	} while (0)


#define SDL_ERR_FUNC() printf("SDL Error %s: %s\n", __func__, SDL_GetError())
#define TTF_ERR_FUNC() printf("TTF Error %s: %s\n", __func__, TTF_GetError())

// typedef struct s_iVec2 {
// 	s32 x;
// 	s32 y;
// } iVec2;

typedef t_vec iVec2;

#define CLEAR_COLOR 70, 70, 70, 255

typedef struct s_sdl_handle {
	SDL_Window		*window;			/* The window ptr */
	SDL_Renderer	*renderer;			/* The renderer ptr */
	iVec2			window_size;
	iVec2			tile_size;
	iVec2			mouse;
}	SDLHandle ;


/* src/sdl_handle */
SDLHandle	*create_sdl_handle(const char* title, u32 y, u32 x);
void		window_clear(SDL_Renderer* renderer);
void		window_close(SDL_Window* window, SDL_Renderer *renderer);
void		draw_color_tile(SDLHandle *h, iVec2 tilePos, iVec2 scale, u32 color);
void		destroy_sdl_handle(SDLHandle *handle);
TTF_Font	*load_font(const char *path, s32 size);
void		unload_font(TTF_Font *font);
void 		write_text(SDLHandle *h, char *text, TTF_Font *font, iVec2 pos, u32 color);

#endif /* HANDLE_SDL_H */
