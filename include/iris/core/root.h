#pragma once

#include <memory>

#include "core/real.h"
#include "graphics/render_system.h"
#include "jobs/job_system.h"
#include "log/logger.h"
#include "physics/physics_system.h"
#include "platform/window.h"

namespace iris
{

/**
 * A singleton object that provides access to various parts of the engine. It
 * controls the construction and destruction of components.
 *
 * This class should provide singleton access to all its components. This means
 * that they themselves do not have to be singletons.
 */
class Root
{
  public:
    ~Root() = default;

    /**
     * Initialises the Root object. Some platforms require deferred
     * initialisation. This *should not* be called directly, the engine
     * will call it when it safe to do so.
     */
    void init();

    /**
     * Get the single instance of root.
     *
     * @returns
     *   Root single instance.
     */
    static Root &instance();

    /**
     * Get single instance of job system.
     *
     * @returns
     *   Job system single instance.
     */
    static JobSystem &job_system();

    /**
     * Get single instance of logger.
     *
     * @returns
     *   Logger single instance.
     */
    static Logger &logger();

    /**
     * Get single instance of physics system.
     *
     * @returns
     *   Render system single instance.
     */
    static PhysicsSystem &physics_system();

    /**
     * Get single instance of render system.
     *
     * @returns
     *   Render system single instance.
     */
    static RenderSystem &render_system();

    /**
     * Get single instance of render window.
     *
     * @returns
     *   Render system single instance.
     */
    static Window &window();

  private:
    /**
     * Private to force access via instance.
     */
    Root();

    /** Single instance. */
    static Root instance_;

    // *NOTE*
    // the order of the members is critical as we need to ensure destruction
    // happens in a fixed order due to dependencies between components
    // e.g. Logger must be destroyed last as other destructors use it

    /** Logger. */
    std::unique_ptr<Logger> logger_;

    /** Job system. */
    std::unique_ptr<JobSystem> job_system_;

    /** Physics system. */
    std::unique_ptr<PhysicsSystem> physics_system_;

    /** Render window. */
    std::unique_ptr<Window> window_;

    /** Render system. */
    std::unique_ptr<RenderSystem> render_system_;
};

}