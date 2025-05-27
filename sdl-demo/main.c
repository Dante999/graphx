#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include <SDL.h>

#define GRAPHX_IMPLEMENTATION
#include "graphx/graphx.h"
#include "graphx/font5x7.h"
#include "graphx/font5x7_test.h"

// Define MAX and MIN macros
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

// Define screen dimensions
#define GFX_WIDTH   480
#define GFX_HEIGHT  800
#define GFX_SIZE   (GRAPHX_BUFFER_SIZE(GFX_WIDTH, GFX_HEIGHT))


#define RENDER_SCALE    2.0f
#define SCREEN_WIDTH    (GFX_WIDTH * RENDER_SCALE)
#define SCREEN_HEIGHT   (GFX_HEIGHT * RENDER_SCALE)


#define INC_Y(Y, FONT_HEIGHT) (Y += FONT_HEIGHT +1)
static void check_sdl(int sdl_error)
{
	if (sdl_error != 0) {
		fprintf(stderr, "SDL_ERROR: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
}


static void set_graphx_buffer_content(struct graphx_data *gfx_data)
{
	graphx_fill(gfx_data, GRAPHX_COLOR_WHITE);

	graphx_draw_rect(gfx_data, 0, 0, gfx_data->width-1, gfx_data->height-1, GRAPHX_COLOR_BLACK);


	uint16_t y = 10;

	uint8_t font_height = font_get_height(font5x7);
	graphx_draw_string(gfx_data, font5x7, 10, y, "Hello World!", GRAPHX_COLOR_BLACK);
	INC_Y(y, font_height);

	graphx_draw_string(gfx_data, font5x7, 10, y, "This is a line with exactly 45 columns width", GRAPHX_COLOR_BLACK);
	INC_Y(y, font_height);

	graphx_draw_hline(gfx_data, 0, y, gfx_data->width-1, GRAPHX_COLOR_BLACK);
	y+=5;

	INC_Y(y, font_height);
	graphx_draw_string(gfx_data, font5x7, 10, y, " !\"#$%&'()*+,-./", GRAPHX_COLOR_BLACK);

	INC_Y(y, font_height);
	graphx_draw_string(gfx_data, font5x7, 10, y, "0123456789:;<=>?", GRAPHX_COLOR_BLACK);

	INC_Y(y, font_height);
	graphx_draw_string(gfx_data, font5x7, 10, y, "@ABCDEFGHIJKLMNO", GRAPHX_COLOR_BLACK);

	INC_Y(y, font_height);
	graphx_draw_string(gfx_data, font5x7, 10, y, "PQRSTUVWXYZ[\\]^_", GRAPHX_COLOR_BLACK);

	INC_Y(y, font_height);
	graphx_draw_string(gfx_data, font5x7, 10, y, "`abcdefghijklmno", GRAPHX_COLOR_BLACK);

	INC_Y(y, font_height);
	graphx_draw_string(gfx_data, font5x7, 10, y, "pqrstuvwxyz{|}~ ", GRAPHX_COLOR_BLACK);

	INC_Y(y, font_height);
	y+=5;
	graphx_draw_hline(gfx_data, 0, y, gfx_data->width-1, GRAPHX_COLOR_BLACK);

	// ------------------

	font_height = font_get_height(font5x7_test);
	graphx_draw_hline(gfx_data, 0, y, gfx_data->width-1, GRAPHX_COLOR_BLACK);
	y+=5;

	INC_Y(y, font_height);
	graphx_draw_string(gfx_data, font5x7_test, 10, y, " !\"#$%&'()*+,-./", GRAPHX_COLOR_BLACK);

	INC_Y(y, font_height);
	graphx_draw_string(gfx_data, font5x7_test, 10, y, "0123456789:;<=>?", GRAPHX_COLOR_BLACK);

	INC_Y(y, font_height);
	graphx_draw_string(gfx_data, font5x7_test, 10, y, "@ABCDEFGHIJKLMNO", GRAPHX_COLOR_BLACK);

	INC_Y(y, font_height);
	graphx_draw_string(gfx_data, font5x7_test, 10, y, "PQRSTUVWXYZ[\\]^_", GRAPHX_COLOR_BLACK);

	INC_Y(y, font_height);
	graphx_draw_string(gfx_data, font5x7_test, 10, y, "`abcdefghijklmno", GRAPHX_COLOR_BLACK);

	INC_Y(y, font_height);
	graphx_draw_string(gfx_data, font5x7_test, 10, y, "pqrstuvwxyz{|}~ ", GRAPHX_COLOR_BLACK);

	INC_Y(y, font_height);
	y+=5;
	graphx_draw_hline(gfx_data, 0, y, gfx_data->width-1, GRAPHX_COLOR_BLACK);
}

static void render_graphx_buffer(SDL_Renderer *renderer, struct graphx_data *gfx_data)
{
	SDL_SetRenderDrawColor(renderer, 0x2F, 0x0F, 0x0F, 0xFF);

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
		.height      = GFX_HEIGHT
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
