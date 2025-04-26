#include "depth.h"
#include "camera.h"
#include "film.h"
#include "interaction.h"
#include "paramset.h"
#include "scene.h"
#include "stats.h"

namespace pbrt {

DepthIntegrator::DepthIntegrator(int maxDepth,
                                 std::shared_ptr<const Camera> camera,
                                 std::shared_ptr<Sampler> sampler,
                                 const Bounds2i& pixelBounds, Float rrThreshold,
                                 const std::string& lightSampleStrategy,
                                 Float minDistance, Float maxDistance, bool use24bit)
    : SamplerIntegrator(camera, sampler, pixelBounds),
      maxDepth(maxDepth),
      rrThreshold(rrThreshold),
      lightSampleStrategy(lightSampleStrategy),
      maxDepthMap(maxDistance),
      minDistance(minDistance),
      maxDistance(maxDistance),
      use24bit(use24bit)  {}

void DepthIntegrator::Preprocess(const Scene& scene, Sampler& sampler) {
    lightDistribution =
        CreateLightSampleDistribution(lightSampleStrategy, scene);
}

Spectrum DepthIntegrator::Li(const RayDifferential& r, const Scene& scene,
                             Sampler& sampler, MemoryArena& arena,
                             int depth) const {
    SurfaceInteraction surfaceInteraction;
    Spectrum L(0.);
    const bool intersect = scene.Intersect(r, &surfaceInteraction);

    if (intersect) {
        auto tmpDepth = r.tMax;

        if (tmpDepth > maxDistance) {
            tmpDepth = maxDistance;
        } else if (tmpDepth < minDistance) {
            tmpDepth = minDistance;
        }

        if (use24bit)
        {
            Float value = ((tmpDepth - minDistance) / (maxDistance - minDistance));

            int intvalue = 0x00FFFFFF * value;

            uint8_t r = intvalue & 0xFF;
            uint8_t g = (intvalue >> 8) & 0xFF;
            uint8_t b = (intvalue >> 16) & 0xFF;

            float rgb[3] = {r / 255.0f, g / 255.0f, b / 255.0f};

            L = L.FromRGB(rgb);
        }
        else
        {
            L = ((tmpDepth - minDistance) / (maxDistance - minDistance));
        }
    }

    return L;
}

DepthIntegrator* CreateDepthIntegrator(const ParamSet& params,
                                       std::shared_ptr<Sampler> sampler,
                                       std::shared_ptr<const Camera> camera) {
    int maxDepth = params.FindOneInt("maxdepth", 5);

    Float minDistance = params.FindOneFloat("mindistance", 0.0);
    Float maxDistance = params.FindOneFloat("maxdistance", 10.0);
    bool use24bit = params.FindOneBool("use24bit", false);

    int np;
    const int* pb = params.FindInt("pixelbounds", &np);
    Bounds2i pixelBounds = camera->film->GetSampleBounds();
    if (pb) {
        if (np != 4)
            Error("Expected four values for \"pixelbounds\" parameter. Got %d.",
                  np);
        else {
            pixelBounds = Intersect(pixelBounds,
                                    Bounds2i{{pb[0], pb[2]}, {pb[1], pb[3]}});
            if (pixelBounds.Area() == 0)
                Error("Degenerate \"pixelbounds\" specified.");
        }
    }
    Float rrThreshold = params.FindOneFloat("rrthreshold", 1.);
    std::string lightStrategy =
        params.FindOneString("lightsamplestrategy", "spatial");
    return new DepthIntegrator(maxDepth, camera, sampler, pixelBounds,
                               rrThreshold, lightStrategy, minDistance, maxDistance, use24bit);
}

}  // namespace pbrt