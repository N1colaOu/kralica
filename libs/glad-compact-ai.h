#pragma once
#if defined(__has_include)
#  if __has_include(<glad/glad.h>)
#    include <glad/glad.h>
#    define NBODY_GLAD_VERSION 1
#  elif __has_include(<glad/gl.h>)
#    include <glad/gl.h>
#    define NBODY_GLAD_VERSION 2
#  else
#    error "Generate a GL 3.3 core GLAD loader into external/glad or let CMake fetch GLAD 2."
#  endif
#else
#  include <glad/glad.h>
#  define NBODY_GLAD_VERSION 1
#endif