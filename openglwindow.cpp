#include "openglwindow.hpp"

#include <imgui.h>
#include <string> 
#include <cstring>
#include <iostream>

#include "abcg.hpp"

void OpenGLWindow::handleEvent(SDL_Event &event) {
  // Teclado
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_SPACE) {
      m_gameData.m_isFiring = true;
    }
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_SPACE) {
      m_gameData.m_isFiring = false;
    }
  }

  // Mouse
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      m_gameData.m_isFiring = true;
    }
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      m_gameData.m_isFiring = false;
    }
  }
  if (event.type == SDL_MOUSEMOTION) {
    glm::ivec2 mousePosition;
    SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

    glm::vec2 direction{glm::vec2{mousePosition.x - m_viewportWidth/2,
                                  mousePosition.y - m_viewportHeight/2}};

    // Normalizando o mouse e passando para o game data
    m_gameData.m_mouseX = direction.x/(m_viewportWidth/2.0f);                   
  }
}

void OpenGLWindow::initializeGL() {
  // Carregando a fonte
  ImGuiIO &io{ImGui::GetIO()};
  auto filename(getAssetsPath() + "Inconsolata-Medium.ttf");
  m_font = io.Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);

  if (m_font == nullptr) {
    throw abcg::Exception{abcg::Exception::Runtime("Nao foi possivel carregar a fonte")};
  }

  // Criando o programa para renderizar os objetos
  m_objectsProgram = createProgramFromFile(getAssetsPath() + "objects.vert",
                                           getAssetsPath() + "objects.frag");

  glClearColor(0, 0, 0, 1);

#if !defined(__EMSCRIPTEN__)
  glEnable(GL_PROGRAM_POINT_SIZE);
#endif

  restart();
}

void OpenGLWindow::restart() {
  m_gameData.m_state = State::Playing;

  m_ship.initializeGL(m_objectsProgram);
  m_base.initializeGL(m_objectsProgram);
  m_aliens.initializeGL(m_objectsProgram);
  m_bullets.initializeGL(m_objectsProgram);
}

void OpenGLWindow::update() {
  float deltaTime{static_cast<float>(getDeltaTime())};

  // Esperar 5 segundos antes de dar restart
  if (m_gameData.m_state != State::Playing && m_restartWaitTimer.elapsed() > 5) {
    restart();
    return;
  }

  if (m_gameData.m_state == State::Playing) {
    checkCollisions();
    checkWinCondition();
  }

  m_ship.update(m_gameData);
  m_aliens.update(deltaTime);
  m_bullets.update(m_ship, m_gameData, deltaTime);
}

void OpenGLWindow::paintGL() {
  update();

  glClear(GL_COLOR_BUFFER_BIT);
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  m_ship.paintGL();
  m_base.paintGL();
  m_aliens.paintGL();
  m_bullets.paintGL();
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  {
    auto size{ImVec2(480, 85)};
    auto position{ImVec2((m_viewportWidth - size.x) / 2.0f,
                         (m_viewportHeight - size.y) / 6.0f)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                           ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoInputs};
    ImGui::Begin(" ", nullptr, flags);
    ImGui::PushFont(m_font);

    if (m_gameData.m_state == State::GameOver) {
      ImGui::TextColored(ImVec4(0,1,0,1), "Voce Perdeu T.T");
    } else if (m_gameData.m_state == State::Win) {
      ImGui::TextColored(ImVec4(0,1,0,1), "Voce venceu! ^_^");
    }

    ImGui::PopFont();
    ImGui::End();
  }
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL() {
  glDeleteProgram(m_objectsProgram);

  m_ship.terminateGL();
  m_base.terminateGL();
  m_aliens.terminateGL();
  m_bullets.terminateGL();
}

void OpenGLWindow::checkCollisions() {
  // Verificando se nenhum alien chegou na base
  float minY = 10.0f;

  for(auto &alien : m_aliens.m_aliens) {
    if(alien.m_translation.y < minY) {
      minY = alien.m_translation.y;
    }
  }

  if(minY <= -0.5f + 0.06f) {
    m_gameData.m_state = State::GameOver;
    m_restartWaitTimer.restart();
  }

  // Verificando se um tiro colidiu com algum alien
  for(auto &bullet : m_bullets.m_bullets) {
    if(bullet.m_dead) {
      continue;
    }

    for(auto &alien : m_aliens.m_aliens) {
      bool yCollides = bullet.m_translation.y > alien.m_translation.y - 0.06f;
      bool xCollides = bullet.m_translation.x > alien.m_translation.x - 0.06f 
                    && bullet.m_translation.x < alien.m_translation.x + 0.06f;

      if (yCollides && xCollides) {
        alien.amtHits++;
        bullet.m_dead = true;
      }
    }
  }

  m_aliens.m_aliens.remove_if(
    [](const Aliens::Alien &a) { 
      return a.amtHits>=a.maxHP; 
    }
  );
}

void OpenGLWindow::checkWinCondition() {
  if(m_aliens.m_aliens.empty()) {
    m_gameData.m_state = State::Win;
    m_restartWaitTimer.restart();
  }
}
