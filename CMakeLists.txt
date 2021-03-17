project(spaceinvaders)

add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp ship.cpp
                               bullets.cpp aliens.cpp bases.cpp)

enable_abcg(${PROJECT_NAME})