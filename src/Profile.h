#ifndef PROFILE_H
#define PROFILE_H

#include <Arduino.h>

// Define loop interval for the control loop (in seconds)
#define LOOP_INTERVAL 0.01  // 10 milliseconds

/**
 * @brief The Profile class manages speed as a function of time or distance.
 *
 * A profile is trapezoidal in shape and consists of up to three phases:
 *   - Acceleration: Speed increases to the maximum speed
 *   - Coasting: Moving at the maximum speed
 *   - Braking: Speed decreases to the final speed
 *
 * The class is unit-agnostic, allowing you to interpret the units as needed,
 * such as mm, cm, or degrees, as long as you're consistent across all parameters.
 */

#define ATOMIC noInterrupts(); // Disable interrupts
#define END_ATOMIC interrupts(); // Re-enable interrupts

class Profile {
 public:
  enum State : uint8_t {
    PS_IDLE = 0,
    PS_ACCELERATING = 1,
    PS_BRAKING = 2,
    PS_FINISHED = 3,
  };

  void reset() {
    ATOMIC {
      m_position = 0;
      m_speed = 0;
      m_target_speed = 0;
      m_state = PS_IDLE;
    }
  }

  bool is_finished() {
    return m_state == PS_FINISHED;
  }

  void start(float distance, float top_speed, float final_speed, float acceleration) {
    m_sign = (distance < 0) ? -1 : +1;
    distance = fabsf(distance);
    if (distance < 1.0) {
      m_state = PS_FINISHED;
      return;
    }
    if (final_speed > top_speed) {
      final_speed = top_speed;
    }

    m_position = 0;
    m_final_position = distance;
    m_target_speed = m_sign * fabsf(top_speed);
    m_final_speed = m_sign * fabsf(final_speed);
    m_acceleration = fabsf(acceleration);
    m_one_over_acc = (m_acceleration >= 1.0) ? 1.0f / m_acceleration : 1.0;
    m_state = PS_ACCELERATING;
  }

  void stop() {
    ATOMIC {
      m_target_speed = 0;
    }
    finish();
  }

  void finish() {
    ATOMIC {
      m_speed = m_target_speed;
      m_state = PS_FINISHED;
    }
  }

  void update() {
    if (m_state == PS_IDLE) return;

    float delta_v = m_acceleration * LOOP_INTERVAL;
    float remaining = fabsf(m_final_position) - fabsf(m_position);

    if (m_state == PS_ACCELERATING && remaining < get_braking_distance()) {
      m_state = PS_BRAKING;
      m_target_speed = (m_final_speed == 0) ? m_sign * 5.0f : m_final_speed;
    }

    // Adjust speed to target speed
    if (m_speed < m_target_speed) {
      m_speed += delta_v;
      if (m_speed > m_target_speed) m_speed = m_target_speed;
    } else if (m_speed > m_target_speed) {
      m_speed -= delta_v;
      if (m_speed < m_target_speed) m_speed = m_target_speed;
    }

    // Increment position
    m_position += m_speed * LOOP_INTERVAL;

    // Mark as finished if within a small threshold
    if (m_state != PS_FINISHED && remaining < 0.125) {
      m_state = PS_FINISHED;
      m_target_speed = m_final_speed;
    }
  }

  float get_braking_distance() {
    return fabsf(m_speed * m_speed - m_final_speed * m_final_speed) * 0.5 * m_one_over_acc;
  }

  float position() {
    float pos;
    ATOMIC { pos = m_position; }
    return pos;
  }

  float speed() {
    float spd;
    ATOMIC { spd = m_speed; }
    return spd;
  }

 private:
  volatile uint8_t m_state = PS_IDLE;
  volatile float m_speed = 0;
  volatile float m_position = 0;
  int8_t m_sign = 1;
  float m_acceleration = 0;
  float m_one_over_acc = 1;
  float m_target_speed = 0;
  float m_final_speed = 0;
  float m_final_position = 0;
};

#endif  // PROFILE_H
