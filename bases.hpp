#ifndef BASES_HPP_
#define BASES_HPP_

#include <list>

#include "abcg.hpp"
#include "gamedata.hpp"

class OpenGLWindow;

class Bases {
  public:
    void initializeGL(GLuint program);
    void paintGL();
    void terminateGL();
  
  private:
    friend OpenGLWindow;

    GLuint m_program{};
    GLint m_colorLoc{};
    GLint m_translationLoc{};
    GLint m_scaleLoc{};

    struct Base {
      GLuint m_vao{};
      GLuint m_vbo{};
      GLuint m_ebo{};

      glm::vec4 m_color{0.848f, 0.604f, 0.032f, 1.0f};
      float m_scale;
      glm::vec2 m_translation{glm::vec2(0)};
    };
    
    std::list<Base> m_bases;

    Bases::Base createBase(glm::vec2 translation);
};

#endif