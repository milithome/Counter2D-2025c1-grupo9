# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/marina/taller/tpg/Counter2D-2025c1-grupo9/_deps/sdl2-src"
  "/home/marina/taller/tpg/Counter2D-2025c1-grupo9/_deps/sdl2-build"
  "/home/marina/taller/tpg/Counter2D-2025c1-grupo9/_deps/sdl2-subbuild/sdl2-populate-prefix"
  "/home/marina/taller/tpg/Counter2D-2025c1-grupo9/_deps/sdl2-subbuild/sdl2-populate-prefix/tmp"
  "/home/marina/taller/tpg/Counter2D-2025c1-grupo9/_deps/sdl2-subbuild/sdl2-populate-prefix/src/sdl2-populate-stamp"
  "/home/marina/taller/tpg/Counter2D-2025c1-grupo9/_deps/sdl2-subbuild/sdl2-populate-prefix/src"
  "/home/marina/taller/tpg/Counter2D-2025c1-grupo9/_deps/sdl2-subbuild/sdl2-populate-prefix/src/sdl2-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/marina/taller/tpg/Counter2D-2025c1-grupo9/_deps/sdl2-subbuild/sdl2-populate-prefix/src/sdl2-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/marina/taller/tpg/Counter2D-2025c1-grupo9/_deps/sdl2-subbuild/sdl2-populate-prefix/src/sdl2-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
