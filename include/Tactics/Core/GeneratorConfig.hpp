#pragma once

namespace Tactics
{
    struct GeneratorConfig
    {
        static constexpr int DEFAULT_WIDTH = 50;
        static constexpr int DEFAULT_HEIGHT = 50;
        static constexpr int DEFAULT_SEED = 42;
        static constexpr float DEFAULT_NOISE_SCALE = 0.05F;
        static constexpr int DEFAULT_NOISE_OCTAVES = 4;
        static constexpr int DEFAULT_CA_ITERATIONS = 3;
        static constexpr float DEFAULT_WATER_THRESHOLD = 0.3F;
        static constexpr float DEFAULT_GRASS_THRESHOLD = 0.5F;
        static constexpr float DEFAULT_FOREST_THRESHOLD = 0.7F;
        static constexpr float DEFAULT_MOUNTAIN_THRESHOLD = 0.85F;

        int width;
        int height;
        int seed;

        float noise_scale;
        int noise_octaves;

        int ca_iterations;

        float water_threshold;
        float grass_threshold;
        float forest_threshold;
        float mountain_threshold;

        [[nodiscard]] static constexpr auto default_config() -> GeneratorConfig
        {
            return GeneratorConfig{.width = DEFAULT_WIDTH,
                                   .height = DEFAULT_HEIGHT,
                                   .seed = DEFAULT_SEED,
                                   .noise_scale = DEFAULT_NOISE_SCALE,
                                   .noise_octaves = DEFAULT_NOISE_OCTAVES,
                                   .ca_iterations = DEFAULT_CA_ITERATIONS,
                                   .water_threshold = DEFAULT_WATER_THRESHOLD,
                                   .grass_threshold = DEFAULT_GRASS_THRESHOLD,
                                   .forest_threshold = DEFAULT_FOREST_THRESHOLD,
                                   .mountain_threshold = DEFAULT_MOUNTAIN_THRESHOLD};
        }
    };
} // namespace Tactics
