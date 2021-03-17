#include "bullets.hpp"

#include <cppitertools/itertools.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Bullets::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");

  m_bullets.clear();

  // Criando um poligono regular
  auto sides{10};

  std::vector<glm::vec2> positions(0);
  positions.emplace_back(0, 0);
  auto step{M_PI * 2 / sides};
  for (auto angle : iter::range(0.0, M_PI * 2, step)) {
    positions.emplace_back(std::cos(angle), std::sin(angle));
  }
  positions.push_back(positions.at(1));

  // Criando um VBO com as posicoes
  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2), positions.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Pegando a localizacao do atributo do program
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  // Criando VAO e fazendo o bind dos atributos do vertice
  glGenVertexArrays(1, &m_vao);

  glBindVertexArray(m_vao);

  glEnableVertexAttribArray(positionAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);
}

void Bullets::paintGL() {
  glUseProgram(m_program);

  glBindVertexArray(m_vao);
  glUniform4f(m_colorLoc, m_color.r, m_color.g, m_color.b, m_color.a);
  glUniform1f(m_scaleLoc, m_scale);

  for (auto &bullet : m_bullets) {
    glUniform2f(m_translationLoc, bullet.m_translation.x, bullet.m_translation.y);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 12);
  }

  glBindVertexArray(0);

  glUseProgram(0);
}

void Bullets::terminateGL() {
  glDeleteBuffers(1, &m_vbo);
  glDeleteVertexArrays(1, &m_vao);
}

void Bullets::update(Ship &ship, const GameData &gameData, float deltaTime) {
  // Cria um par de tiros
  if (gameData.m_isFiring && gameData.m_state == State::Playing) {
    // So permite tiros uma vez a cada 250ms
    if (ship.m_bulletCoolDownTimer.elapsed() > 250.0 / 1000.0) {
      ship.m_bulletCoolDownTimer.restart();

      glm::vec2 cannonOffset{(6.0f / 15.0f) * ship.m_scale, 0.0f};

      Bullet bullet{.m_dead = false,
                    .m_translation = ship.m_translation + cannonOffset,
                    .m_velocity = glm::vec2{0.0f, 2.0f}};
      m_bullets.push_back(bullet);

      bullet.m_translation = ship.m_translation - cannonOffset;
      m_bullets.push_back(bullet);
    }
  }

  for (auto &bullet : m_bullets) {
    bullet.m_translation += bullet.m_velocity * deltaTime;

    // Mata as balas que sairem do viewport
    if (
      bullet.m_translation.x < -1.1f || 
      bullet.m_translation.x > +1.1f ||
      bullet.m_translation.y < -1.1f ||
      bullet.m_translation.y > +1.1f 
    ) 
    {
      bullet.m_dead = true;
    }
  }

  // Remove as balas mortas
  m_bullets.remove_if([](const Bullet &b) { return b.m_dead; });
}