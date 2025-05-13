#include "testutils.h"

#define GRAPHX_IMPLEMENTATION
#include "graphx/graphx.h"

#include <stdio.h>

static void test_graphx_verify__nullptr(void)
{
	TEST_ASSERT_EQ_INT(GRAPHX_ERROR_NULLPTR, graphx_verify(NULL));
}

static void test_graphx_verify__buffer_null(void)
{
	struct graphx_data data = {
		.buffer = NULL,
		.width  = 128,
		.height = 64
	};

	TEST_ASSERT_EQ_INT(GRAPHX_ERROR_BUFFER_NULL, graphx_verify(&data));
}

static void test_graphx_verify__invalid_buffer_size(void)
{
	const uint16_t width       = 128;
	const uint16_t height      = 64;
	const uint16_t buffer_size = GRAPHX_BUFFER_SIZE(width,height);
	uint8_t buffer[buffer_size];

	struct graphx_data data = {
		.buffer      = buffer,
		.buffer_size = buffer_size+10, // bigger than expected
		.width       = width,
		.height      = height
	};

	TEST_ASSERT_EQ_INT(GRAPHX_ERROR_INVALID_BUFFER_SIZE, graphx_verify(&data));
}

static void test_graphx_verify__happycase(void)
{
	const uint16_t width       = 128;
	const uint16_t height      = 64;
	const uint16_t buffer_size = GRAPHX_BUFFER_SIZE(width,height);
	uint8_t buffer[buffer_size];

	struct graphx_data data = {
		.buffer      = buffer,
		.buffer_size = buffer_size,
		.width       = width,
		.height      = height
	};

	TEST_ASSERT_EQ_INT(GRAPHX_ERROR_OK, graphx_verify(&data));
}

static void test_graphx_fill__all_white_then_black_then_toggle(void)
{
	const uint16_t width       = 128;
	const uint16_t height      = 64;
	const uint16_t buffer_size = GRAPHX_BUFFER_SIZE(width,height);
	uint8_t buffer[buffer_size];

	struct graphx_data data = {
		.buffer      = buffer,
		.buffer_size = buffer_size,
		.width       = width,
		.height      = height
	};

	TEST_ASSERT_EQ_INT(GRAPHX_ERROR_OK, graphx_verify(&data));

	graphx_fill(&data, GRAPHX_COLOR_WHITE);
	TEST_ASSERT_MEMCMP(0x00, buffer, buffer_size);

	graphx_fill(&data, GRAPHX_COLOR_BLACK);
	TEST_ASSERT_MEMCMP(0xFF, buffer, buffer_size);

	graphx_fill(&data, GRAPHX_COLOR_TOGGLE);
	TEST_ASSERT_MEMCMP(0x00, buffer, buffer_size);
}

static void test_graphx_draw_pixel__white_then_black_then_toggle(void)
{
	const uint16_t width       = 128;
	const uint16_t height      = 64;
	const uint16_t buffer_size = GRAPHX_BUFFER_SIZE(width,height);
	uint8_t buffer[buffer_size];

	struct graphx_data data = {
		.buffer      = buffer,
		.buffer_size = buffer_size,
		.width       = width,
		.height      = height
	};

	TEST_ASSERT_EQ_INT(GRAPHX_ERROR_OK, graphx_verify(&data));

	graphx_fill(&data, GRAPHX_COLOR_WHITE);
	TEST_ASSERT_MEMCMP(0x00, buffer, buffer_size);

	graphx_draw_pixel(&data, 0, 0, GRAPHX_COLOR_BLACK);
	TEST_ASSERT_MEMCMP(0x01, buffer, 1);

	graphx_draw_pixel(&data, 8, 0, GRAPHX_COLOR_BLACK);
	TEST_ASSERT_MEMCMP(0x01, buffer, 1);
}

int main(void)
{
	RUNTEST(test_graphx_verify__nullptr);
	RUNTEST(test_graphx_verify__buffer_null);
	RUNTEST(test_graphx_verify__invalid_buffer_size);
	RUNTEST(test_graphx_verify__happycase);
	RUNTEST(test_graphx_fill__all_white_then_black_then_toggle);
	RUNTEST(test_graphx_draw_pixel__white_then_black_then_toggle);


	return 0;
}
