#include "Tactics/Core/ZoomController.hpp"
#include "Tactics/Core/Camera.hpp"
#include "Tactics/Core/InputManager.hpp"

namespace Tactics
{
    void ZoomController::update(Camera &camera, float delta_time)
    {
        auto &input = InputManager::instance();

        bool zoom_out_just_pressed = input.is_key_just_pressed(SDL_SCANCODE_Q);
        bool zoom_in_just_pressed = input.is_key_just_pressed(SDL_SCANCODE_E);
        bool zoom_out_pressed = input.is_key_pressed(SDL_SCANCODE_Q);
        bool zoom_in_pressed = input.is_key_pressed(SDL_SCANCODE_E);

        if (zoom_in_just_pressed)
        {
            m_zoom_repeat_timer = KEY_REPEAT_INITIAL_DELAY;
            zoom_in(camera);

            return;
        }

        if (zoom_out_just_pressed)
        {
            m_zoom_repeat_timer = KEY_REPEAT_INITIAL_DELAY;
            zoom_out(camera);

            return;
        }

        if (!zoom_in_pressed && !zoom_out_pressed)
        {
            return;
        }

        m_zoom_repeat_timer -= delta_time;

        if (m_zoom_repeat_timer <= 0.0F)
        {
            m_zoom_repeat_timer = KEY_REPEAT_RATE;

            if (zoom_in_pressed)
            {
                zoom_in(camera);
            }

            if (zoom_out_pressed)
            {
                zoom_out(camera);
            }
        }
    }

    void ZoomController::zoom_in(Camera &camera)
    {
        camera.set_zoom(camera.get_zoom() * ZOOM_IN_FACTOR);
    }

    void ZoomController::zoom_out(Camera &camera)
    {
        camera.set_zoom(camera.get_zoom() * ZOOM_OUT_FACTOR);
    }
} // namespace Tactics
