#include "aliens.hpp"

void Aliens::initializeGL(GLuint program) {
  terminateGL();

  // Valores iniciais das translacoes
  float xInit = -0.84f;
  float yInit = 0.85f;

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");

  // Criando os aliens
  m_aliens.clear();
  m_aliens.resize(m_qtdLinhas * m_qtdColunas);

  glm::vec2 translationStep{xInit, yInit};

  // Variaveis auxiliares para atribuir um translationStep correto
  int col = 0;
  int lin = 0;

  for(auto &alien : m_aliens) {
    alien = createAlien(translationStep, lin, col);

    col++;
    translationStep.x += 0.14;

    if (col == m_qtdColunas) {
      col = 0;
      lin++;

      translationStep.x = xInit;
      translationStep.y -= 0.14;
    }
  }

  // Inicializando o rng
  auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_rnjesus.seed(seed);

  // Gerando a lista de offsets de velocidades
  std::uniform_real_distribution<float> spdOffDist(0.0f, 0.5f);

  for(long long unsigned int i = 0; i < colSpdOff.size(); i++) {
    colSpdOff.at(i) = spdOffDist(m_rnjesus);
  }
}

void Aliens::paintGL() {
  glUseProgram(m_program);

  for(auto &alien : m_aliens) {
    glBindVertexArray(alien.m_vao);

    glUniform4f(m_colorLoc, alien.m_color.r, alien.m_color.g, alien.m_color.b, alien.m_color.a);
    glUniform1f(m_scaleLoc, alien.m_scale);
    glUniform2f(m_translationLoc, alien.m_translation.x, alien.m_translation.y);

    glDrawElements(GL_TRIANGLES, alien.amtVert, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
  }

  glUseProgram(0);
}

void Aliens::update(float deltaTime) {
  // Atualizando a posicao de cada alien
  for(auto &alien : m_aliens) {
    alien.m_translation.y -= ((m_fallSpd + colSpdOff.at(alien.col))/60) * deltaTime;
  }
}

void Aliens::terminateGL() {
  for (auto alien : m_aliens) {
    glDeleteBuffers(1, &alien.m_vbo);
    glDeleteVertexArrays(1, &alien.m_vao);
  }
}

Aliens::Alien Aliens::createAlien(glm::vec2 translation, int linha, int coluna) { 
  if (linha == 0) {
    return createStrongAlien(translation, coluna);
  } else if (linha <= 2) {
    return createMediumAlien(translation, coluna);
  } else {
    return createWeakAlien(translation, coluna);
  }
}

Aliens::Alien Aliens::createStrongAlien(glm::vec2 translation, int coluna) { 
  Alien alien;

  alien.m_translation = translation;
  alien.maxHP = 10;
  alien.col = coluna;
  alien.m_color = glm::vec4{0.896f, 0.244f, 0.196f, 1.0f};

  // Os valores ja estao normalizados
  std::array<glm::vec2, 32> positions{
    // Antena esquerda
    glm::vec2{-0.6f, 1.0f}, glm::vec2{-0.2f, 1.0f}, // 0, 1,
    glm::vec2{-0.2f, 0.8f}, glm::vec2{-0.6f, 0.8f}, // 2, 3,

    // Antena direita
    glm::vec2{0.2f, 1.0f}, glm::vec2{0.6f, 1.0f}, // 4, 5,
    glm::vec2{0.6f, 0.8f}, glm::vec2{0.2f, 0.8f}, // 6, 7,

    // Corpo
    glm::vec2{-0.8f, 0.8f}, glm::vec2{0.8f, 0.8f},   // 8, 9,
    glm::vec2{0.8f, 0.6f}, glm::vec2{-0.8f, 0.6f},   // 10, 11,
    glm::vec2{-1.0f, 0.6f}, glm::vec2{1.0f, 0.6f},   // 12, 13,
    glm::vec2{1.0f, -0.6f}, glm::vec2{-1.0f, -0.6f}, // 14, 15,

    // Perna esquerda
    glm::vec2{-0.8f, -0.6f}, glm::vec2{-0.6f, -0.6f}, // 16, 17,
    glm::vec2{-0.6f, -1.0f}, glm::vec2{-0.8f, -1.0f}, // 18, 19,

    // Perna meio-esquerda
    glm::vec2{-0.4f, -0.6f}, glm::vec2{-0.2f, -0.6f}, // 20, 21,
    glm::vec2{-0.2f, -1.0f}, glm::vec2{-0.4f, -1.0f}, // 22, 23,

    // Perna meio-direita
    glm::vec2{0.2f, -0.6f}, glm::vec2{0.4f, -0.6f}, // 24, 25,
    glm::vec2{0.4f, -1.0f}, glm::vec2{0.2f, -1.0f}, // 26, 27,

    // Perna meio-esquerda
    glm::vec2{0.6f, -0.6f}, glm::vec2{0.8f, -0.6f}, // 28, 29,
    glm::vec2{0.8f, -1.0f}, glm::vec2{0.6f, -1.0f}, // 30, 31,
  };

  std::array indices{
                    // Antenas
                     0,  1,  2,
                     0,  2,  3,
                     4,  5,  6,
                     4,  6,  7,
                    // Corpo
                     8,  9, 10,
                     8, 10, 11,
                    12, 13, 14,
                    12, 14, 15,
                    // Pernas
                    16, 17, 18,
                    16, 18, 19,
                    20, 21, 22,
                    20, 22, 23,
                    24, 25, 26,
                    24, 26, 27,
                    28, 29, 30,
                    28, 30, 31,
                    };

  alien.amtVert = indices.size();

  // Montando o VBO
  glGenBuffers(1, &alien.m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, alien.m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Montando o EBO
  glGenBuffers(1, &alien.m_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, alien.m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Pegando a location dos atributos no programa
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  // Criando VAO
  glGenVertexArrays(1, &alien.m_vao);

  // Dando bind nos atributos dos vertices com o VAO atual
  glBindVertexArray(alien.m_vao);

  glEnableVertexAttribArray(positionAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, alien.m_vbo);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, alien.m_ebo);

  // Finalizando o bind
  glBindVertexArray(0);

  return alien;
}

Aliens::Alien Aliens::createMediumAlien(glm::vec2 translation, int coluna) { 
  Alien alien;

  alien.m_translation = translation;
  alien.maxHP = 6;
  alien.col = coluna;
  alien.m_color = glm::vec4{0.492f, 0.212f, 0.664f, 1.0f};

  // Os valores ja estao normalizados
  std::array<glm::vec2, 21> positions{
    // Corpo
    glm::vec2{0.0f, 1.0f},                           // 0,
    glm::vec2{-0.6f, 0.8f}, glm::vec2{0.6f, 0.8f},   // 1, 2,
    glm::vec2{1.0f, -0.2f}, glm::vec2{-1.0f, -0.2f}, // 3, 4,
    glm::vec2{-0.8f, -0.4f}, glm::vec2{0.8f, -0.4f}, // 5, 6,

    // Perna esquerda
    glm::vec2{-0.6f, -0.4f},                          // 7,
    glm::vec2{-0.8f, -1.0f}, glm::vec2{-1.0f, -1.0f}, // 8, 9,

    // Perna meio-esquerda
    glm::vec2{-0.4f, -0.4f}, glm::vec2{-0.2f, -0.4f}, // 10, 11,
    glm::vec2{-0.4f, -1.0f}, glm::vec2{-0.6f, -1.0f}, // 12, 13,

    // Perna meio-direita
    glm::vec2{0.2f, -0.4f}, glm::vec2{0.4f, -0.4f}, // 14, 15,
    glm::vec2{0.6f, -1.0f}, glm::vec2{0.4f, -1.0f}, // 16, 17,

    // Perna direita
    glm::vec2{0.6f, -0.4f},                         // 18,
    glm::vec2{1.0f, -1.0f}, glm::vec2{0.8f, -1.0f}, // 19, 20,
  };

  std::array indices{
                    // Corpo
                     0,  1,  2,
                     1,  2,  3,
                     1,  3,  4,
                     3,  4,  6,
                     4,  5,  6,
                    // Pernas
                     5,  7,  8,
                     5,  8,  9,
                    10, 11, 12,
                    10, 12, 13,
                    14, 15, 16,
                    14, 16, 17,
                     6, 18, 19,
                    18, 19, 20,
                    };

  alien.amtVert = indices.size();                  

  // Montando o VBO
  glGenBuffers(1, &alien.m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, alien.m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Montando o EBO
  glGenBuffers(1, &alien.m_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, alien.m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Pegando a location dos atributos no programa
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  // Criando VAO
  glGenVertexArrays(1, &alien.m_vao);

  // Dando bind nos atributos dos vertices com o VAO atual
  glBindVertexArray(alien.m_vao);

  glEnableVertexAttribArray(positionAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, alien.m_vbo);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, alien.m_ebo);

  // Finalizando o bind
  glBindVertexArray(0);

  return alien;
}

Aliens::Alien Aliens::createWeakAlien(glm::vec2 translation, int coluna) { 
  Alien alien;

  alien.m_translation = translation;
  alien.maxHP = 4;
  alien.col = coluna;
  alien.m_color = glm::vec4{0.888f, 0.864f, 0.196f, 1.0f};

  // Os valores ja estao normalizados
  std::array<glm::vec2, 40> positions{
    // Corpo
    glm::vec2{-0.6f, 0.6f}, glm::vec2{0.6f, 0.6f},   //  0,  1,
    glm::vec2{0.6f, 0.4f}, glm::vec2{-0.6f, 0.4f},   //  2,  3,
    glm::vec2{-0.8f, 0.4f}, glm::vec2{0.8f, 0.4f},   //  4,  5,
    glm::vec2{0.8f, -0.4f}, glm::vec2{-0.8f, -0.4f}, //  6,  7,
    glm::vec2{-0.6f, -0.4f}, glm::vec2{0.6f, -0.4f}, //  8,  9,
    glm::vec2{0.6f, -0.6f}, glm::vec2{-0.6f, -0.6f}, // 10, 11,

    // Antena Esquerda
    glm::vec2{-0.28f, 1.0f}, glm::vec2{-0.12f, 1.0f}, // 12, 13,
    glm::vec2{-0.12f, 0.6f}, glm::vec2{-0.28f, 0.6f}, // 14, 15,

    // Antena Direita
    glm::vec2{0.12f, 1.0f}, glm::vec2{0.28f, 1.0f}, // 16, 17,
    glm::vec2{0.28f, 0.6f}, glm::vec2{0.12f, 0.6f}, // 18, 19,

    // Braco Esquerdo
    glm::vec2{-0.8f, 0.2f}, glm::vec2{-0.8f, 0.0f},   // 20, 21,
    glm::vec2{-1.0f, -0.4f}, glm::vec2{-1.0f, -0.2f}, // 22, 23,

    // Braco Direito
    glm::vec2{0.8f, 0.2f}, glm::vec2{1.0f, -0.2f}, // 24, 25,
    glm::vec2{1.0f, -0.4f}, glm::vec2{0.8f, 0.0f}, // 26, 27,

    // Perna esquerda
    glm::vec2{-0.36f, -0.6f}, glm::vec2{-0.2f, -0.6f}, // 28, 29,
    glm::vec2{-0.4f, -0.8f}, glm::vec2{-0.56f, -0.8f}, // 30, 31,
    glm::vec2{-0.4f, -1.0f}, glm::vec2{-0.56f, -1.0f}, // 32, 33,

    // Perna direita
    glm::vec2{0.36f, -0.6f}, glm::vec2{0.2f, -0.6f}, // 34, 35,
    glm::vec2{0.4f, -0.8f}, glm::vec2{0.56f, -0.8f}, // 36, 37,
    glm::vec2{0.4f, -1.0f}, glm::vec2{0.56f, -1.0f}, // 38, 39,
  };

  std::array indices{
                    // Corpo
                     0,  1,  2,
                     0,  2,  3,
                     4,  5,  6,
                     4,  6,  7,
                     8,  9, 10,
                     8, 10, 11,
                    // Antenas
                    12, 13, 14,
                    12, 14, 15,
                    16, 17, 18,
                    16, 18, 19,
                    // Bracos
                    20, 21, 22,
                    20, 22, 23,
                    24, 25, 26,
                    24, 26, 27,
                    //Pernas
                    28, 29, 30,
                    28, 30, 31,
                    30, 31, 32,
                    31, 32, 33,
                    34, 35, 37,
                    35, 36, 37,
                    36, 37, 39,
                    36, 38, 39,
                    };

  alien.amtVert = indices.size();

  // Montando o VBO
  glGenBuffers(1, &alien.m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, alien.m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Montando o EBO
  glGenBuffers(1, &alien.m_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, alien.m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Pegando a location dos atributos no programa
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  // Criando VAO
  glGenVertexArrays(1, &alien.m_vao);

  // Dando bind nos atributos dos vertices com o VAO atual
  glBindVertexArray(alien.m_vao);

  glEnableVertexAttribArray(positionAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, alien.m_vbo);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, alien.m_ebo);

  // Finalizando o bind
  glBindVertexArray(0);

  return alien;
}
