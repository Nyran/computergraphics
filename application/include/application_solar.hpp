#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"
#include "options.hpp"

#include "planet.hpp"
#include <array>
#include "texture_loader.hpp"



// gpu representation of model
class ApplicationSolar : public Application {
 public:
  // allocate and initialize objects
  ApplicationSolar(std::string const& resource_path);
  // free allocated objects
  ~ApplicationSolar();

  // update uniform locations and values
  void uploadUniforms();
  // update projection matrix
  void updateProjection();
  // react to key input
  void keyCallback(int key, int scancode, int action, int mods);
  // handle delta mouse movement input
  void mouseCallback(double pos_x, double pos_y);
  // draw all objects
  void render() const;


  // calculate and upload model- and normal-matrix
  void upload_planet_transforms(Planet const& planet) const;

  // upload stars
  void upload_stars() const;

  // create and upload planet circle
  std::vector<float> create_orbit(int const depth) const;
  void upload_orbit(Planet const& planet) const;

  // upload skymap
  void upload_skymap() const;

 protected:
  void initializeShaderPrograms();
  void initializeTextures();
  void initializeFramebuffer();
  void initializeGeometry();
  void updateView();

  // cpu representation of model
  model_object planet_object;
  model_object star_object;
  model_object orbit_object;
  model_object squad_object;

  // texture object
  texture_object tex_object;

  //texture object for framebuffer
  GLuint tex_handle;

  //renderbuffer object
  GLuint rb_handle;

  //framebuffer object
  GLuint fbo_handle;

  // planet container
  std::array<std::shared_ptr<Planet>, 11> planets;

  // current shader
  std::string shader = "planet";

  // render options
  Options renderopts;
};

#endif