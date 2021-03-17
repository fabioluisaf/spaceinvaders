#include "ship.hpp"

void Ship::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");

  m_translation = glm::vec2(0.0f, -0.85f);

  std::array<glm::vec2, 12> positions{
    // Corpo da nave
    glm::vec2{0, 12.0f}, glm::vec2{-15.0f, -15.0f},
    glm::vec2{0, -7.0f}, glm::vec2{15.0f, -15.0f},

    // Canhao direito
    glm::vec2{5.0f, 6.0f}, glm::vec2{7.0f, 6.0f},
    glm::vec2{7.0f, -13.0f}, glm::vec2{5.0f, -13.0f},

    // Canhao esquerdo
    glm::vec2{-7.0f, 6.0f}, glm::vec2{-5.0f, 6.0f},
    glm::vec2{-5.0f, -13.0f}, glm::vec2{-7.0f, -13.0f},
  };

  // Normalizando
  for (auto &position :positions) {
    position /= glm::vec2{15.0f, 15.0f};
  }

  std::array indices{
                     // Corpo
                     0,1,2,
                     0,2,3,
                     // Canhao dir
                     4,5,6,
                     4,6,7,
                     //Canhao esq
                     8,9,10,
                     8,10,11,
                    };

  // Montando o VBO
  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Montando o EBO
  glGenBuffers(1, &m_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Pegando a location dos atributos no programa
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  // Criando VAO
  glGenVertexArrays(1, &m_vao);

  // Dando bind nos atributos dos vertices com o VAO atual
  glBindVertexArray(m_vao);

  glEnableVertexAttribArray(positionAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

  // Finalizando o bind
  glBindVertexArray(0);
}

void Ship::paintGL() {
  glUseProgram(m_program);

  glBindVertexArray(m_vao);

  glUniform1f(m_scaleLoc, m_scale);
  glUniform2fv(m_translationLoc, 1, &m_translation.x);

  glUniform4fv(m_colorLoc, 1, &m_color.r);
  glDrawElements(GL_TRIANGLES, 6 * 3, GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);

  glUseProgram(0);
}

void Ship::terminateGL() {
  glDeleteBuffers(1, &m_vbo);
  glDeleteBuffers(1, &m_ebo);
  glDeleteVertexArrays(1, &m_vao);
}

void Ship::update(const GameData &gameData) {
  if (gameData.m_state != State::Playing) return;
  
  // Movendo a nave
  m_translation.x = gameData.m_mouseX;

  // Impedindo a nave de sair da tela
  if (m_translation.x + m_scale > 1) {
    m_translation.x = 1 - m_scale;
  }

  if (m_translation.x - m_scale < -1) {
    m_translation.x = -1 + m_scale;
  }
}