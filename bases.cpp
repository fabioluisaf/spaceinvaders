#include "bases.hpp"

void Bases::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");

  // Criando a base aliada
  m_bases.clear();
  m_bases.resize(5);

  glm::vec2 translationStep{-0.8f, -0.6f};

  for(auto &base : m_bases) {
    base = createBase(translationStep);
    translationStep.x += 0.4;
  }
}

void Bases::paintGL() {
  glUseProgram(m_program);

  for(auto &base : m_bases) {
    glBindVertexArray(base.m_vao);

    glUniform4f(m_colorLoc, base.m_color.r, base.m_color.g, base.m_color.b, base.m_color.a);
    glUniform1f(m_scaleLoc, base.m_scale);
    glUniform2f(m_translationLoc, base.m_translation.x, base.m_translation.y);

    glDrawElements(GL_TRIANGLES, 6*3, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
  }

  glUseProgram(0);
}

void Bases::terminateGL() {
  for (auto base : m_bases) {
    glDeleteBuffers(1, &base.m_vbo);
    glDeleteVertexArrays(1, &base.m_vao);
  }
}

Bases::Base Bases::createBase(glm::vec2 translation) {
  Base base;

  base.m_translation = translation;
  base.m_scale = 1.0f;

  // Os valores ja estao normalizados
  std::array<glm::vec2, 12> positions{
    // Quadrado superior
    glm::vec2{-0.1f, 0.1f}, glm::vec2{0.1f, 0.1f},
    glm::vec2{0.1f, 0.0f}, glm::vec2{-0.1f, 0.0f},

    // Perna direita
    glm::vec2{0.04f, 0.1f}, glm::vec2{0.1f, 0.1f},
    glm::vec2{0.1f, -0.1f}, glm::vec2{0.04f, -0.1f},

    // Perna esquerda
    glm::vec2{-0.04f, 0.1f}, glm::vec2{-0.1f, 0.1f},
    glm::vec2{-0.1f, -0.1f}, glm::vec2{-0.04f, -0.1f},
  };

  std::array indices{
                    // Quadrado superior
                    0,1,2,
                    0,2,3,
                    // Perna direita
                    4,5,6,
                    4,6,7,
                    //Perna esquerda
                    8,9,10,
                    8,10,11,
                  };

  // Montando o VBO
  glGenBuffers(1, &base.m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, base.m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Montando o EBO
  glGenBuffers(1, &base.m_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, base.m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Pegando a location dos atributos no programa
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  // Criando VAO
  glGenVertexArrays(1, &base.m_vao);

  // Dando bind nos atributos dos vertices com o VAO atual
  glBindVertexArray(base.m_vao);

  glEnableVertexAttribArray(positionAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, base.m_vbo);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, base.m_ebo);

  // Finalizando o bind
  glBindVertexArray(0);

  return base;
}