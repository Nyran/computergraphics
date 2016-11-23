#ifndef PLANET_HPP
#define PLANET_HPP

#include <memory>

// Super sweet planet struct by AwesomeTek
struct Planet {
  Planet() {};
  Planet(float p_size, float p_dist, float p_rot_speed, float p_orb_speed, bool p_orbit, glm::fvec3 p_color, std::string p_texture)
    : size{p_size}
    , distance{p_dist}
    , rotation_speed{p_rot_speed}
	, orbital_speed{p_orb_speed}
    , orbit{ p_orbit }
	, color{p_color}
	, texture{p_texture}
  {}
  Planet(Planet const& p_planet)
	: size{ p_planet.size }
	, distance{ p_planet.distance }
	, rotation_speed{ p_planet.rotation_speed }
	, orbital_speed{ p_planet.orbital_speed }
	, orbit{ p_planet.orbit }
	, color{p_planet.color}
	, texture{p_planet.texture}
  {}

  float size = 0;						// Diameter of Planet
  float distance = 0;					// Distance to Sun
  float rotation_speed = 0;				// Rotation speed
  float orbital_speed = 0;				// Speed of Planet
  bool orbit = 0;						// Has orbit?
  std::shared_ptr<Planet> parent;		// Pointer to parent (usually the sun)
  glm::fvec3 color = {0.0, 0.0, 0.0};	// Color of planetsurface
  std::string texture = "none";			// Name of texture file	
  int textureID;						// Test	
};

#endif //PLANET_HPP
