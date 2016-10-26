#ifndef PLANET_HPP
#define PLANET_HPP

#include <memory>

// Super sweet planet struct by AwesomeTek
struct Planet {
  Planet() {};
  Planet(float p_size, float p_dist, float p_rot_speed, float p_orb_speed, bool p_orbit)
    : size{p_size}
    , distance{p_dist}
    , rotation_speed{p_rot_speed}
	  , orbital_speed{p_orb_speed}
    , orbit{ p_orbit }
  {}
  Planet(Planet const& p_planet)
	  : size{ p_planet.size }
	  , distance{ p_planet.distance }
	  , rotation_speed{ p_planet.rotation_speed }
	  , orbital_speed{ p_planet.orbital_speed }
    , orbit{ p_planet.orbit }
  {}

  float size = 0;           // Diameter of Planet
  float distance = 0;       // Distance to Sun
  float rotation_speed = 0; // Rotation speed
  float orbital_speed = 0;  // Speed of Planet
  bool orbit = 0;           // Has orbit?
  std::shared_ptr<Planet> parent;
};

#endif //PLANET_HPP
