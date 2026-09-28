#include <camera-ai.h>
#include <renderer-ai.h>
#include <shader-ai.h>
#include <window-ai.h>

// ---- your physics code, used verbatim ----
#include <body.h>
#include <constants.h>
#include <forcefields.h>
#include <integrators.h>
#include <simulation.h>
#include <vector.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <vector>

#ifndef SHADER_DIR
#  define SHADER_DIR "shaders"
#endif

using namespace kralica;

namespace {

struct Scenario {
    std::string       name;
    std::vector<Body> bodies;
    double            dt;
    int               stepsPerFrame;
    float             cameraDistance;
    float             cameraPitch;
};


std::vector<Body> makeBinary() {
    const double m = 1000.0;   // heavy enough to be visible with G = 6.67430e-3
    const double d = 0.60;    // separation
    const double v = std::sqrt(G * m / (2.0 * d));   // circular-orbit speed

    std::vector<Body> out;
    out.emplace_back(Vector2d({-0.5 * d,  0.0}), Vector2d({0.0, -v}), m);
    out.emplace_back(Vector2d({ 0.5 * d,  0.0}), Vector2d({0.0,  v}), m);
    return out;
}

std::vector<Body> makeCluster(int n, double radius, double mass, unsigned seed) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<double> u(-radius, radius);

    std::vector<Body> out;
    out.reserve(n);
    for (int i = 0; i < n; ++i) {
        const double x = u(rng);
        const double y = u(rng);
        out.emplace_back(Vector2d({x, y}), Vector2d({0.0, 0.0}), mass);
    }
    return out;
}

std::vector<Body> makeGalaxy(int n, double radius, double coreMass,
                             double particleMass, unsigned seed) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<double> u01(0.0, 1.0);
    std::uniform_real_distribution<double> uang(0.0, 6.283185307179586);

    std::vector<Body> out;
    out.reserve(n + 1);

    // Central mass
    out.emplace_back(Vector2d({0.0, 0.0}), Vector2d({0.0, 0.0}), coreMass);

    for (int i = 0; i < n; ++i) {
        const double r  = radius * std::sqrt(u01(rng));   // uniform in a disk
        const double th = uang(rng);
        const double x  = r * std::cos(th);
        const double y  = r * std::sin(th);

        const double v  = std::sqrt(G * coreMass / std::max(r, 0.02));
        const double vx = -v * std::sin(th);
        const double vy =  v * std::cos(th);

        out.emplace_back(Vector2d({x, y}), Vector2d({vx, vy}), particleMass);
    }
    return out;
}

std::vector<Scenario> buildScenarios() {
    std::vector<Scenario> out;

    out.push_back({
        "Binary Star",
        makeBinary(),
        1e-3,    // dt
        60,      // steps per frame
        3.5f,    // camera distance
        0.0f,    // pitch (straight-on 2D view)
    });

    out.push_back({
        "Cold Collapse",
        makeCluster(400, 0.70, 0.5, 42u),
        5e-4, 40, 3.0f, 0.0f,
    });

    out.push_back({
        "Galaxy",
        makeGalaxy(1500, 0.85, 3000.0, 0.5, 7u),
        5e-4, 50, 3.2f, 0.0f,
    });

    return out;
}

void printHelp() {
    std::cout <<
        "\nControls\n"
        "  Left mouse drag      orbit\n"
        "  Right / middle drag  pan\n"
        "  Scroll wheel         zoom\n"
        "  Space                pause / resume\n"
        "  1..3                 select scenario\n"
        "  N / P                next / previous scenario\n"
        "  R                    reload current scenario\n"
        "  [ / ]                halve / double steps per frame\n"
        "  - / =                shrink / grow points\n"
        "  Esc                  quit\n\n";
}

} // namespace

int main() {
    try {
        Window window(1280, 960, "2D N-Body Visualizer");

        Renderer renderer;
        renderer.init(SHADER_DIR);

        Camera camera;
        camera.setViewport(window.width(), window.height());

        std::vector<Scenario> scenarios = buildScenarios();
        int current = 0;

        // Objects that must outlive the Simulation.
        std::unique_ptr<CompositeField> field;
        std::unique_ptr<Simulation>     sim;

        auto load = [&](int idx) {
            const int count = static_cast<int>(scenarios.size());
            idx = ((idx % count) + count) % count;
            current = idx;

            Scenario& sc = scenarios[idx];

            // Destroy the old simulation before the old force field.
            sim.reset();
            field.reset();

            // -------- force field: gravity only (add LennardJonesForce here if you like)
            std::vector<std::unique_ptr<ForceField>> ffs;
            ffs.push_back(std::make_unique<GravityForce>());
            field = std::make_unique<CompositeField>(std::move(ffs));

            // -------- initial accelerations: run force computation once so the
            // first Verlet step has real accelerations to work with.
            std::vector<Vector2d> accs(sc.bodies.size(), Vector2d({0.0, 0.0}));
            field->compute(sc.bodies, accs);

            // -------- integrator and simulation
            std::unique_ptr<Integrator> integrator =
                std::make_unique<VerletIntegrator>();

            sim = std::make_unique<Simulation>(sc.bodies, accs,
                                               integrator, *field, sc.dt);

            // -------- camera
            camera.setTarget(glm::vec3(0.0f));
            camera.setDistance(sc.cameraDistance);
            camera.setAngles(0.0f, sc.cameraPitch);

            std::cout << "Loaded '" << sc.name << "' ("
                      << sc.bodies.size() << " bodies)\n";
        };

        load(current);
        printHelp();

        bool  paused       = false;
        float pointScale   = 1.0f;
        int   stepsPerFrame = scenarios[current].stepsPerFrame;

        double lastTime    = window.time();
        double fpsAccum    = 0.0;
        int    framesAccum = 0;

        while (!window.shouldClose()) {
            window.newFrame();

            camera.setViewport(window.width(), window.height());
            glViewport(0, 0, window.width(), window.height());

            const double now = window.time();
            const float  frameTime = static_cast<float>(std::min(now - lastTime, 0.1));
            lastTime = now;

            // ----------------------------------------------------------- input
            const InputState& in = window.input();
            if (in.leftMouseDown) {
                camera.orbit(static_cast<float>(in.mouseDeltaX),
                             static_cast<float>(in.mouseDeltaY));
            }
            if (in.rightMouseDown || in.middleMouseDown) {
                camera.pan(static_cast<float>(in.mouseDeltaX),
                           static_cast<float>(in.mouseDeltaY));
            }
            if (in.scrollDeltaY != 0.0) {
                camera.zoom(static_cast<float>(in.scrollDeltaY));
            }

            if (window.wasKeyPressed(GLFW_KEY_ESCAPE)) window.close();
            if (window.wasKeyPressed(GLFW_KEY_SPACE))  paused = !paused;
            if (window.wasKeyPressed(GLFW_KEY_R))      load(current);
            if (window.wasKeyPressed(GLFW_KEY_N))      load(current + 1);
            if (window.wasKeyPressed(GLFW_KEY_P))      load(current - 1);

            for (int i = 0; i < static_cast<int>(scenarios.size()) && i < 9; ++i) {
                if (window.wasKeyPressed(GLFW_KEY_1 + i)) load(i);
            }

            if (window.wasKeyPressed(GLFW_KEY_LEFT_BRACKET))
                stepsPerFrame = std::max(1, stepsPerFrame / 2);
            if (window.wasKeyPressed(GLFW_KEY_RIGHT_BRACKET))
                stepsPerFrame = std::min(4000, stepsPerFrame * 2);
            if (window.wasKeyPressed(GLFW_KEY_MINUS))
                pointScale = std::max(0.25f, pointScale * 0.8f);
            if (window.wasKeyPressed(GLFW_KEY_EQUAL))
                pointScale = std::min(8.0f,  pointScale * 1.25f);

            // ------------------------------------------------------ simulation
            if (!paused) {
                for (int i = 0; i < stepsPerFrame; ++i) sim->step();
            }

            // ------------------------------------------------------- rendering
            renderer.upload(sim->get_bodies());
            renderer.draw(camera, pointScale);

            window.swapBuffers();

            // ------------------------------------------------------------- hud
            ++framesAccum;
            fpsAccum += frameTime;
            if (fpsAccum >= 0.5) {
                char title[256];
                std::snprintf(title, sizeof(title),
                    "2D N-Body | %s | %zu bodies | %.1f FPS | %d steps/frame | t=%.3f%s",
                    scenarios[current].name.c_str(),
                    sim->get_bodies().size(),
                    framesAccum / fpsAccum,
                    stepsPerFrame,
                    sim->get_time(),
                    paused ? " | PAUSED" : "");
                window.setTitle(title);
                fpsAccum    = 0.0;
                framesAccum = 0;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}