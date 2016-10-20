#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"

#include "planet.hpp"
#include <array>

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

 protected:
  void initializeShaderPrograms();
  void initializeGeometry();
  void updateView();

  // cpu representation of model
  model_object planet_object;

  // planet container
  std::array<std::shared_ptr<Planet>, 11> planets;
};

#endif