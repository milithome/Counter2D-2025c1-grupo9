# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/marina/taller/tpg/Counter2D-2025c1-grupo9/_deps/sdl2_ttf-src"
  "/home/marina/taller/tpg/Counter2D-2025c1-grupo9/_deps/sdl2_ttf-build"
  "/home/marina/taller/tpg/Counter2D-2025c1-grupo9/_deps/sdl2_ttf-subbuild/sdl2_ttf-populate-prefix"
  "/home/marina/taller/tpg/Counter2D-2025c1-grupo9/_deps/sdl2_ttf-subbuild/sdl2_ttf-populate-prefix/tmp"
  "/home/marina/taller/tpg/Counter2D-2025c1-grupo9/_deps/sdl2_ttf-subbuild/sdl2_ttf-populate-prefix/src/sdl2_ttf-populate-stamp"
  "/home/marina/taller/tpg/Counter2D-2025c1-grupo9/_deps/sdl2_ttf-subbuild/sdl2_ttf-populate-prefix/src"
  "/home/marina/taller/tpg/Counter2D-2025c1-grupo9/_deps/sdl2_ttf-subbuild/sdl2_ttf-populate-prefix/src/sdl2_ttf-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/marina/taller/tpg/Counter2D-2025c1-grupo9/_deps/sdl2_ttf-subbuild/sdl2_ttf-populate-prefix/src/sdl2_ttf-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/marina/taller/tpg/Counter2D-2025c1-grupo9/_deps/sdl2_ttf-subbuild/sdl2_ttf-populate-prefix/src/sdl2_ttf-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
