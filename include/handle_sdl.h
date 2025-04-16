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

/* Macro to convert RGBA to UINT32 and UINT32 to RGBA */
#define UINT32_TO_RGBA(color, r, g, b, a) \
    do { \
        (r) = ((color) >> 24) & 0xFF; \
        (g) = ((color) >> 16) & 0xFF; \
        (b) = ((color) >> 8) & 0xFF; \
        (a) = (color) & 0xFF; \
    } while (0)

#define RGBA_TO_UINT32(r, g, b, a) \
	((r) << 24 | (g) << 16 | (b) << 8 | (a))

#define SDL_ERR_FUNC() ft_printf_fd(2, "SDL Error %s: %s\n", __func__, SDL_GetError())
#define TTF_ERR_FUNC() ft_printf_fd(2, "TTF Error %s: %s\n", __func__, TTF_GetError())

typedef t_vec iVec2;

/* Clear color */
#define U32_CLEAR_COLOR (0x464646FF)

/* Font path */
#define ARIAL_FONT_PATH "./rsc/font/arial.ttf"

/* Font size */
#define INFO_FONT_SIZE	15
#define BIG_FONT_SIZE	45

/* Boolean flag for get_str_pixel_len funct */
#define GET_X 0
#define GET_Y 1

typedef struct s_sdl_handle {
	SDL_Window		*window;			/* The window ptr */
	SDL_Renderer	*renderer;			/* The renderer ptr */
	iVec2			window_size;		/* The window size */
	iVec2			tile_size;			/* The tile size */
	iVec2			mouse;				/* The mouse position */
	TTF_Font		*font;				/* The font */
	TTF_Font		*big_font;			/* The big font (for pause display) */
}	SDLHandle ;


/* src/sdl_handle */
SDLHandle	*create_sdl_handle(const char* title, u32 y, u32 x);
void		window_clear(SDL_Renderer* renderer, u32 color);
void		window_close(SDL_Window* window, SDL_Renderer *renderer);
void		draw_color_tile(SDLHandle *h, iVec2 tilePos, iVec2 scale, u32 color);
void		destroy_sdl_handle(SDLHandle *handle);
TTF_Font	*load_font(const char *path, s32 size);
void		unload_font(TTF_Font *font);
void 		write_text(SDLHandle *h, char *text, TTF_Font *font, iVec2 pos, u32 color);
int			get_str_pixel_len(char *str, TTF_Font *font, s8 flag);

#endif /* HANDLE_SDL_H */
