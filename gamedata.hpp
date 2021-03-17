#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

enum class State { Playing, GameOver, Win };

struct GameData {
  State m_state{State::Playing};
  bool m_isFiring = false;
  float m_mouseX = 0.0f;
};

#endif
