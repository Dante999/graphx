
all: clean font_generator fonts graphx_demo

font_generator:
	make -C font-generator -B all

graphx_demo:
	make -C graphx-demo -B all

fonts:
#	./font-generator/font-generator parse_atlas \
#		--input_file=font-atlas/font_3x5.png \
#		--char_width=3 \
#		--char_height=5 \
#		--output_file=include/graphx/font3x5.h

#	./font-generator/font-generator parse_atlas \
#		--input_file=font-atlas/font_5x7.png \
#		--char_width=5 \
#		--char_height=7 \
#		--output_file=include/graphx/font5x7.h

	./font-generator/font-generator parse_atlas \
		--input_file=font-atlas/font_10x14.png \
		--char_width=10 \
		--char_height=14 \
		--output_file=include/graphx/font10x14.h


clean:
	make -C font-generator clean
	make -C graphx-demo clean


