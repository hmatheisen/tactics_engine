#pragma once

#include <SDL3/SDL.h>

namespace Tactics
{
    class Scene
    {
    public:
        Scene() = default;
        virtual ~Scene() = default;

        // Delete copy constructor and assignment operator
        Scene(const Scene &) = delete;
        auto operator=(const Scene &) -> Scene & = delete;

        // Delete move constructor and assignment operator
        Scene(Scene &&) = delete;
        auto operator=(Scene &&) -> Scene & = delete;

        // Called when scene is entered
        [[nodiscard]] virtual auto on_enter() -> bool
        {
            return true;
        }

        // Called when scene is exited
        virtual void on_exit() {}

        // Update scene logic
        virtual void update(float delta_time) = 0;

        // Render scene
        virtual void render(SDL_Renderer *renderer) = 0;

        // Check if scene should transition
        [[nodiscard]] virtual auto should_exit() const -> bool
        {
            return false;
        }
    };
} // namespace Tactics
