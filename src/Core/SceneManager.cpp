#include "Tactics/Core/SceneManager.hpp"
#include "Tactics/Core/Logger.hpp"

namespace Tactics
{
    SceneManager::SceneManager() = default;

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    void SceneManager::push_scene(std::unique_ptr<Scene> scene)
    {
        if (scene == nullptr)
        {
            log_warning("Attempted to push null scene");
            return;
        }

        // Exit current scene if it exists
        if (!m_scene_stack.empty())
        {
            m_scene_stack.top()->on_exit();
        }

        // Enter new scene
        if (!scene->on_enter())
        {
            log_error("Failed to enter new scene");
            return;
        }

        m_scene_stack.push(std::move(scene));
        log_debug("Scene pushed, total scenes: " + std::to_string(m_scene_stack.size()));
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    void SceneManager::pop_scene()
    {
        if (m_scene_stack.empty())
        {
            log_warning("Attempted to pop scene from empty stack");
            return;
        }

        m_scene_stack.top()->on_exit();
        m_scene_stack.pop();

        // Enter previous scene if it exists
        if (!m_scene_stack.empty())
        {
            m_scene_stack.top()->on_enter();
        }

        log_debug("Scene popped, remaining scenes: " + std::to_string(m_scene_stack.size()));
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    void SceneManager::change_scene(std::unique_ptr<Scene> scene)
    {
        if (scene == nullptr)
        {
            log_warning("Attempted to change to null scene");
            return;
        }

        // Exit current scene if it exists
        if (!m_scene_stack.empty())
        {
            m_scene_stack.top()->on_exit();
            m_scene_stack.pop();
        }

        // Enter new scene
        if (!scene->on_enter())
        {
            log_error("Failed to enter new scene");
            return;
        }

        m_scene_stack.push(std::move(scene));
        log_debug("Scene changed, total scenes: " + std::to_string(m_scene_stack.size()));
    }

    void SceneManager::update(float delta_time)
    {
        if (m_scene_stack.empty())
        {
            return;
        }

        Scene *current_scene = nullptr;
        current_scene = m_scene_stack.top().get();
        if (current_scene == nullptr)
        {
            return;
        }

        current_scene->update(delta_time);

        // Check if scene wants to exit
        if (current_scene->should_exit())
        {
            pop_scene();
        }
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    void SceneManager::render(SDL_Renderer *renderer)
    {
        if (m_scene_stack.empty() || renderer == nullptr)
        {
            return;
        }

        Scene *current_scene = nullptr;
        current_scene = m_scene_stack.top().get();
        if (current_scene == nullptr)
        {
            return;
        }

        current_scene->render(renderer);
    }

    auto SceneManager::is_running() const -> bool
    {
        return !m_scene_stack.empty();
    }
} // namespace Tactics
