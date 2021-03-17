#ifndef ALIENS_HPP_
#define ALIENS_HPP_

#include <list>
#include <random>

#include "abcg.hpp"
#include "gamedata.hpp"

class OpenGLWindow;

class Aliens {
  public:
    void initializeGL(GLuint program);
    void paintGL();
    void terminateGL();

    void update(float deltaTime); 

  private:
    friend OpenGLWindow;
    
    GLuint m_program{};
    GLint m_colorLoc{};
    GLint m_translationLoc{};
    GLint m_scaleLoc{};

    // Dimensoes da matriz de aliens
    static const int m_qtdLinhas{5};
    static const int m_qtdColunas{13};

    // Velocidade base de descida dos aliens
    static const int m_fallSpd{1};

    struct Alien {
      GLuint m_vao{};
      GLuint m_vbo{};
      GLuint m_ebo{};

      glm::vec4 m_color{1};
      glm::vec2 m_translation{glm::vec2(0)};

      float m_scale{0.06f};
      int amtHits{0};
      int maxHP{};
      int col{};
      long long unsigned int amtVert{};
    };
    
    std::list<Alien> m_aliens;
    std::array<float, m_qtdColunas> colSpdOff; // faz com que cada coluna da matriz
                                // se mova numa velocidade diferente

    std::default_random_engine m_rnjesus;

    Aliens::Alien createAlien(glm::vec2 translation, int linha, int coluna);
    Aliens::Alien createStrongAlien(glm::vec2 translation, int coluna);
    Aliens::Alien createMediumAlien(glm::vec2 translation, int coluna);
    Aliens::Alien createWeakAlien(glm::vec2 translation, int coluna);
};

#endif