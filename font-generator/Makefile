#LIBCUTILS_SRC_DIR=../thirdparty/libcutils/src
#LIBCUTILS_INC_DIR=../thirdparty/libcutils/include

CC_FLAGS = -Wall -Wextra -Werror -Wpedantic -g \
	-I../include
#	-I${LIBCUTILS_INC_DIR} \

LD_FLAGS = -lm



all:
	${CC} ${CC_FLAGS} -o font-generator \
		main.c \
		atlas_generator.c \
		atlas_parser.c \
	${LD_FLAGS}

clean:
	rm -rf font-generator
