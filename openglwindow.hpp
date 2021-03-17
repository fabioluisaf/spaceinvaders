#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <imgui.h>

#include "abcg.hpp"
#include "aliens.hpp"
#include "bullets.hpp"
#include "ship.hpp"
#include "bases.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& event) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  GLuint m_objectsProgram{};

  int m_viewportWidth{};
  int m_viewportHeight{};
  float minimY{};

  GameData m_gameData;

  Ship m_ship;
  Bases m_base;
  Aliens m_aliens;
  Bullets m_bullets;

  abcg::ElapsedTimer m_restartWaitTimer;

  ImFont* m_font{};

  void restart();
  void update();
  void checkCollisions();
  void checkWinCondition();
};

#endif