#include "../../include/handle_sdl.h"
#include "../../libft/libft.h"

/**
 * @brief Create a window with SDL2
 * @param width The width of the window
 * @param height The height of the window
 * @param title The title of the window
 * @return The window pointer
*/
SDL_Window* create_sdl_windows(SDLHandle *h, const char* title) {
	SDL_Window		*window = NULL;
	SDL_Renderer	*renderer = NULL;
	
	/* Init SDL2 and TTF */
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		SDL_ERR_FUNC();
		return (NULL);
	}

	if (TTF_Init() != 0) {
		TTF_ERR_FUNC();
		SDL_Quit();
		return (NULL);
	}

	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, h->window_size.x, h->window_size.y, SDL_WINDOW_SHOWN);
	if (!window) {
		SDL_ERR_FUNC();
		return (NULL);
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		SDL_ERR_FUNC();
		SDL_DestroyWindow(window);
		return (NULL);
	}

	/* Enable blending mode to handle alpha */
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	return (window);
}


// static TTF_Font *safe_load_font(SDLHandle *h, const char *path, s32 fontSize) {
// 	TTF_Font *font = load_font(path, fontSize);
// 	if (!font) {
// 		SDL_DestroyRenderer(h->renderer);
// 		SDL_DestroyWindow(h->window);
// 		free(h);
// 		ft_printf_fd(2, "%s: load_font %s failed\n", __func__, path);
// 		return (NULL);
// 	}
// 	return (font);
// }

/**
 * @brief Create a SDLHandle
 * @param width The width of the window
 * @param height The height of the window
 * @param title The title of the window
 * @param board The chess board
 * @return The SDLHandle pointer
*/
SDLHandle *create_sdl_handle(const char* title, u32 y, u32 x) {
	SDLHandle *handle = ft_calloc(sizeof(SDLHandle),1);
	if (!handle) {
		ft_printf_fd(2, "%s : malloc failed\n", __func__);
		return (NULL);
	} 
	
	handle->window_size.x = x;
	handle->window_size.y = y;

	if (!(handle->window = create_sdl_windows(handle, title))) {
		free(handle);
		return (NULL);
	} else if (!(handle->renderer = SDL_GetRenderer(handle->window))) {
		SDL_ERR_FUNC();
		SDL_DestroyWindow(handle->window);
		free(handle);
		return (NULL);
	} 
	window_clear(handle->renderer);
	return (handle);
}


/**
 * @brief Clear the window with SDL2
 * @param window The window pointers
*/
void window_clear(SDL_Renderer* renderer) {
	// SDL_SetRenderDrawColor(renderer, CLEAR_COLOR);
	SDL_SetRenderDrawColor(renderer, 0, 0, 120, 255);
	SDL_RenderClear(renderer);
}

/**
 * @brief Close the window with SDL2
 * @param window The window pointers
 * @note We need to destroy renderer before destroying the window
 */
void window_close(SDL_Window* window, SDL_Renderer *renderer) {

	if (renderer) {
		SDL_DestroyRenderer(renderer);
	}
	if (window) {
		SDL_DestroyWindow(window);
		TTF_Quit();
		SDL_Quit();
	}
}

/**
 * @brief Draw a color tile with SDL2
 * @param window The window pointers
 * @param tile_pos, The position of the tile
 * @param scale The scale of the tile
 * @note If scale.x/y are equal to TILE_SIZE, we use TILE_SPACING to space the tiles
*/
void draw_color_tile(SDLHandle *h, iVec2 tile_pos, iVec2 scale, u32 color) {
	SDL_Rect		tileRect = {0,0,0,0};
	iVec2			pixel_pos = {0,0};

	(void)color;
	u8 				r, g, b, a;
	UINT32_TO_RGBA(color, r, g, b, a);

	/* Convert tile coordinates to pixel coordinates */
	// ft_printf_fd(1, "Draw tile at %u, %u\n", tile_pos.y, tile_pos.x);
	pixel_pos.x = tile_pos.x;
	pixel_pos.y = tile_pos.y;

	tileRect.x = pixel_pos.x; tileRect.y = pixel_pos.y;
	tileRect.w = scale.x; tileRect.h = scale.y;

	/* Set the drawing color and draw the tile */
	SDL_SetRenderDrawColor(h->renderer, r, g, b, a);
	// SDL_SetRenderDrawColor(h->renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(h->renderer, &tileRect);
}

/**
 * @brief Unload a texture with SDL2
 * @param texture The texture pointer
*/
void unload_texture(SDL_Texture *texture) {
	if (!texture) {
		return ;
	}
	SDL_DestroyTexture(texture);
}

void destroy_sdl_handle(SDLHandle *handle) {
	if (!handle) {
		return ;
	}
	/* Close window */
	window_close(handle->window, handle->renderer);

}


/**
 * @brief Load a font with SDL2
 * @param path The path of the font
 * @return The font pointer
*/
TTF_Font *load_font(const char *path, s32 fontSize) {
	TTF_Font *font = TTF_OpenFont(path, fontSize);
	if (!font) {
		return (NULL);
	}
	return (font);
}

/**
 * @brief Unload a font with SDL2
 * @param font The font pointer
*/
void unload_font(TTF_Font *font) {
	if (font) {
		TTF_CloseFont(font);
	}
}

/**
 * @brief Write a text with SDL2
 * @param window The window pointers
 * @param font The font pointer
 * @param text The text to write
 * @param pos The position of the text
 * @param fontSize The size of the text
 * @param color The color of the text
*/
void write_text(SDLHandle *h, char *text, TTF_Font *font, iVec2 pos, u32 color) {
	SDL_Surface		*surface = NULL;
	SDL_Texture		*texture = NULL;
	SDL_Rect		textRect = {0,0,0,0};
	u8 				r, g, b, a;

	if (!h->window || !font) {
		return;
	}

	UINT32_TO_RGBA(color, r, g, b, a);

	surface = TTF_RenderText_Solid(font, text, (SDL_Color){r, g, b, a});
	if (!surface) {
		ft_printf_fd(2, "%s\n", SDL_GetError());
		return;
	}
	texture = SDL_CreateTextureFromSurface(h->renderer, surface);
	if (!texture) {
		ft_printf_fd(2, "%s\n", SDL_GetError());
		SDL_FreeSurface(surface);
		return;
	}

	textRect.x = pos.x;
	textRect.y = pos.y;
	textRect.w = surface->w;
	textRect.h = surface->h;
	SDL_RenderCopy(h->renderer, texture, NULL, &textRect);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}