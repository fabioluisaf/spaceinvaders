#ifndef SHIP_HPP_
#define SHIP_HPP_

#include "abcg.hpp"
#include "gamedata.hpp"

class Aliens;
class Bullets;
class OpenGLWindow;
class Bases;

class Ship {
 public:
  void initializeGL(GLuint program);
  void paintGL();
  void terminateGL();

  void update(const GameData &gameData);

 private:
  friend Aliens;
  friend Bullets;
  friend OpenGLWindow;
  friend Bases;

  GLuint m_program{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};
  GLint m_colorLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};
  GLuint m_ebo{};

  glm::vec4 m_color{0.0862f, 0.6862f, 0.8705f, 1.0f};
  glm::vec2 m_translation{glm::vec2(0)};
  float m_scale{0.06f};

  abcg::ElapsedTimer m_bulletCoolDownTimer;
};

#endif