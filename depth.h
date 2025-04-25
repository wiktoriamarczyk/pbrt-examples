#ifndef PBRT_INTEGRATORS_DEPTH_H
#define PBRT_INTEGRATORS_DEPTH_H

#include "pbrt.h"
#include "integrator.h"
#include "../core/lightdistrib.h"

namespace pbrt {

    // DepthIntegrator Declarations
    class DepthIntegrator : public SamplerIntegrator {
    public:
        // DepthIntegrator Public Methods
        DepthIntegrator(int maxDepth, std::shared_ptr<const Camera> camera,
            std::shared_ptr<Sampler> sampler,
            const Bounds2i& pixelBounds, Float rrThreshold = 1,
                      const std::string& lightSampleStrategy = "spatial",
                      Float minDistance = 0.0, Float maxDistance = 10.0, bool use24bit = false);

        void Preprocess(const Scene& scene, Sampler& sampler);
        Spectrum Li(const RayDifferential& ray, const Scene& scene,
            Sampler& sampler, MemoryArena& arena, int depth) const;

    private:
        // DepthIntegrator Private Data
        const int maxDepth;
        const float maxDepthMap;
        const Float rrThreshold;
        const std::string lightSampleStrategy;
        std::unique_ptr<LightDistribution> lightDistribution;

        const Float minDistance;
        const Float maxDistance;
        const bool use24bit;
    };

    DepthIntegrator* CreateDepthIntegrator(
        const ParamSet& params, std::shared_ptr<Sampler> sampler,
        std::shared_ptr<const Camera> camera);

}  // namespace pbrt

#endif  // PBRT_INTEGRATORS_DEPTH_H
