#ifndef RENDERER_H
#define RENDERER_H

#include "Scene.h"

//----------------------------------------------------------------------------------------------------------------------
/// @brief this class is designed purely to provide interface for any onject that is going to render our scene
/// any class that implements this interface would implicaidly expect that the following methods are called in the following order
/// setScene()
/// initialize()
/// render()
//----------------------------------------------------------------------------------------------------------------------
class Renderer {

public:
    Renderer():m_scene(NULL) { }
    ~Renderer() { }

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief pure virtual initialize method we override in our subclass class to do our drawing
    /// this is only called one time, just after we have a valid GL context use this to init any global GL elements
    //----------------------------------------------------------------------------------------------------------------------
    virtual void initialize()=0;

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief pure virtual render method we override in our base class to do our drawing, called every update
    //----------------------------------------------------------------------------------------------------------------------
    virtual void render()=0;

    const Scene* getScene() const { return m_scene; }

    void setScene(Scene* _scene) { m_scene = _scene; }

protected:

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief reference to our scene
    /// note: renderer is not the owner of this object so it should not care about destroying it
    /// note: whenever scene is changed initializition and render should be retrigered
    //----------------------------------------------------------------------------------------------------------------------
    Scene* m_scene;
};

#endif // RENDERER_H
