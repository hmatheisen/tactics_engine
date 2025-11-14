#pragma once

#include <SDL3/SDL.h>
#include <Tactics/Core/Scene.hpp>
#include <memory>
#include <stack>

namespace Tactics
{
    class SceneManager
    {
    public:
        SceneManager();
        ~SceneManager() = default;

        // Delete copy constructor and assignment operator
        SceneManager(const SceneManager &) = delete;
        auto operator=(const SceneManager &) -> SceneManager & = delete;

        // Delete move constructor and assignment operator
        SceneManager(SceneManager &&) = delete;
        auto operator=(SceneManager &&) -> SceneManager & = delete;

        // Push a new scene (current scene stays in stack)
        void push_scene(std::unique_ptr<Scene> scene);

        // Pop current scene (return to previous)
        void pop_scene();

        // Replace current scene
        void change_scene(std::unique_ptr<Scene> scene);

        // Update current scene
        void update(float delta_time);

        // Render current scene
        void render(SDL_Renderer *renderer);

        // Check if we should quit (no scenes left)
        [[nodiscard]] auto is_running() const -> bool;

    private:
        std::stack<std::unique_ptr<Scene>> m_scene_stack;
    };
} // namespace Tactics
