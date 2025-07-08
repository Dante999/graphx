#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include <SDL.h>

#define GRAPHX_IMPLEMENTATION
#include "graphx/graphx.h"

#include "graphx/font3x5.h"
#include "graphx/font5x7.h"
#include "graphx/font10x14.h"


// Define screen dimensions
#define GFX_WIDTH   400
#define GFX_HEIGHT  400
#define GFX_SIZE   (GRAPHX_BUFFER_SIZE(GFX_WIDTH, GFX_HEIGHT))


#define RENDER_SCALE    3.0f
#define SCREEN_WIDTH    (GFX_WIDTH * RENDER_SCALE)
#define SCREEN_HEIGHT   (GFX_HEIGHT * RENDER_SCALE)


static void check_sdl(int sdl_error)
{
	if (sdl_error != 0) {
		fprintf(stderr, "SDL_ERROR: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
}


static uint16_t draw_font_atlas(
		struct graphx_data *gfx_data,
		const uint8_t *font,
		const uint16_t x_start,
		const uint16_t y_start)
{
	size_t char_width      = font_get_width(font);
	size_t char_height     = font_get_height(font);
	size_t font_array_len  = font_get_data_len(font);
	printf("char_width=%zu char_height=%zu data_len=%zu\n",
		char_width, char_height, font_array_len);

	size_t offset_per_char = char_width * (char_height / 8 + 1);

	uint16_t x = x_start;
	uint16_t y = y_start;

	graphx_draw_hline(gfx_data, 0, y, gfx_data->width-1, GRAPHX_COLOR_BLACK);
	y += 5;

	size_t symbol_count = font_array_len/offset_per_char;
	assert(symbol_count < 255);

	x = x_start;
	for (size_t i=0; i< symbol_count; ++i) {
		printf("demo: drawing symbol with index=%03zu at x=%03d y=%03d\n", i, x, y);
		graphx_draw_symbol(gfx_data, font, x, y, (char)i, GRAPHX_COLOR_BLACK);
		x += char_width+1;

		if ( (i+1) % 16 == 0) {
			printf("demo: -> new row\n");
			x = x_start;
			y += char_height+1;
		}
	}

	y += char_height+5;

	graphx_draw_hline(gfx_data, 0, y, gfx_data->width-1, GRAPHX_COLOR_BLACK);

	return y;
}

static void set_graphx_buffer_content(struct graphx_data *gfx_data)
{
	graphx_fill(gfx_data, GRAPHX_COLOR_WHITE);

	graphx_draw_rect(gfx_data, 0, 0, gfx_data->width-1, gfx_data->height-1, GRAPHX_COLOR_BLACK);

	const uint16_t x_start = 10;
	uint16_t y = 10;
	y = draw_font_atlas(gfx_data, font3x5,   x_start, y);
	y = draw_font_atlas(gfx_data, font5x7,   x_start, y);
	y = draw_font_atlas(gfx_data, font10x14, x_start, y);
}

static void render_graphx_buffer(SDL_Renderer *renderer, struct graphx_data *gfx_data)
{
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);

	for (uint16_t x=0; x < gfx_data->width; ++x) {
		for (uint16_t y=0; y < gfx_data->height; ++y) {

			enum graphx_color pixel = graphx_get_pixel(gfx_data, x, y);

			if (pixel == GRAPHX_COLOR_BLACK) {
				SDL_RenderDrawPoint(renderer, x, y);
			}
		}
	}
}

int main(int argc, char* argv[])
{
	uint8_t gfx_buffer[GFX_SIZE];

	struct graphx_data gfx_data = {
		.buffer      = gfx_buffer,
		.buffer_size = sizeof(gfx_buffer),
		.width       = GFX_WIDTH,
		.height      = GFX_HEIGHT,
		.orientation = GRAPHX_ORIENTATION_VERTICAL
	};

	set_graphx_buffer_content(&gfx_data);

	enum graphx_error gerror = graphx_verify(&gfx_data);
	if (gerror != GRAPHX_ERROR_OK) {
		fprintf(stderr, "ERROR: Failed to initialize graphx: %d\n", gerror);
		return -1;
	}

	printf("using graphx_buffer with: w=%d h=%d size=%d\n",
			gfx_data.width, gfx_data.height, gfx_data.buffer_size);
	// Unused argc, argv
	(void) argc;
	(void) argv;

	// Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not be initialized!\n"
				"SDL_Error: %s\n", SDL_GetError());
		return 0;
	}

#if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
	// Disable compositor bypass
	if(!SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0"))
	{
		printf("SDL can not disable compositor bypass!\n");
		return 0;
	}
#endif

	// Create window
	SDL_Window *window = SDL_CreateWindow("Graphx SDL Demo",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH, SCREEN_HEIGHT,
			SDL_WINDOW_SHOWN);
	if(!window)
	{
		printf("Window could not be created!\n"
				"SDL_Error: %s\n", SDL_GetError());
	}
	else
	{
		// Create renderer
		SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		if(!renderer)
		{
			printf("Renderer could not be created!\n"
				"SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
			SDL_RenderSetScale(renderer, RENDER_SCALE, RENDER_SCALE);

			// Event loop exit flag
			bool quit = false;

			// Event loop
			while(!quit)
			{
				SDL_Event e;

				// Wait indefinitely for the next available event
				SDL_WaitEvent(&e);

				// User requests quit
				if(e.type == SDL_QUIT)
				{
					quit = true;
				}

				// Initialize renderer color white for the background
				check_sdl(SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF));

				// Clear screen
				check_sdl(SDL_RenderClear(renderer));

				render_graphx_buffer(renderer, &gfx_data);

				// Update screen
				SDL_RenderPresent(renderer);
			}

			// Destroy renderer
			SDL_DestroyRenderer(renderer);
		}

		// Destroy window
		SDL_DestroyWindow(window);
	}

	// Quit SDL
	SDL_Quit();

	return 0;
}
