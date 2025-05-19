def Settings( **kwargs ):
  return { 'flags': ['-Wall', 'Wextra', '-Wpedantic',
                     '-Iinclude',
                     '-I/usr/include/SDL2',
                     '-Ithirdparty/libcutils/include'
  ] }

