#LIBCUTILS_SRC_DIR=../thirdparty/libcutils/src
#LIBCUTILS_INC_DIR=../thirdparty/libcutils/include

CC_FLAGS = -Wall -Wextra -Werror -Wpedantic \
	-I../include
#	-I${LIBCUTILS_INC_DIR} \

all:
	${CC} ${CC_FLAGS} -o font-generator \
		main.c \
		template_generator.c

clean:
	rm font-generator
