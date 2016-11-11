#include "application_solar.hpp"
#include "launcher.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding 
using namespace gl;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <stack>
#include <random>

ApplicationSolar::ApplicationSolar(std::string const& resource_path)
 :Application{resource_path}
 ,planet_object{}
 ,star_object{}
 ,orbit_object{}
{
  initializeGeometry();
  initializeShaderPrograms();
}

void ApplicationSolar::render() const{
  /*
  // bind shader to upload uniforms
  glUseProgram(m_shaders.at("planet").handle);

  glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime()), glm::fvec3{0.0f, 2.0f, 0.0f});
  model_matrix = glm::translate(model_matrix, glm::fvec3{0.0f, 0.0f, -1.0f});
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                     1, GL_FALSE, glm::value_ptr(model_matrix));

  // extra matrix for normal transformation to keep them orthogonal to surface
  glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                     1, GL_FALSE, glm::value_ptr(normal_matrix));

  // bind the VAO to draw
  glBindVertexArray(planet_object.vertex_AO);

  // draw bound vertex array using bound shader
  glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
  */
  for (auto planet : planets) {
    upload_planet_transforms(*planet);
  }

  upload_stars();
}

void ApplicationSolar::updateView() {
  // vertices are transformed in camera space, so camera transform must be inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);
  // upload matrix to gpu
  glUseProgram(m_shaders.at(shader).handle);
  glUniformMatrix4fv(m_shaders.at(shader).u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));
  glUseProgram(m_shaders.at("star").handle);
  glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ViewMatrix"),
    1, GL_FALSE, glm::value_ptr(view_matrix));
  glUseProgram(m_shaders.at("orbit").handle);
  glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ViewMatrix"),
    1, GL_FALSE, glm::value_ptr(view_matrix));
}

void ApplicationSolar::updateProjection() {
  // upload matrix to gpu
  glUseProgram(m_shaders.at(shader).handle);
  glUniformMatrix4fv(m_shaders.at(shader).u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));
  glUseProgram(m_shaders.at("star").handle);
  glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ProjectionMatrix"),
    1, GL_FALSE, glm::value_ptr(m_view_projection));
  glUseProgram(m_shaders.at("orbit").handle);
  glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ProjectionMatrix"),
    1, GL_FALSE, glm::value_ptr(m_view_projection));

//  glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ProjectionMatrix"),
//	  1, GL_FALSE, glm::value_ptr(m_view_projection));
}

// update uniform locations
void ApplicationSolar::uploadUniforms() {
  updateUniformLocations();
  
  // bind new shader
  //glUseProgram(m_shaders.at("planet").handle);
  //glUseProgram(m_shaders.at("star").handle);
  
  updateView();
  updateProjection();
}

// handle key input
void ApplicationSolar::keyCallback(int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action== GLFW_PRESS)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.1f});
    updateView();
  }
  else if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.1f});
    updateView();
  }
  else if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{ -0.1f, 0.0f, 0.0f });
    updateView();
  }
  else if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{ 0.1f, 0.0f, 0.0f });
    updateView();
  }
  else if (key == GLFW_KEY_E && action == GLFW_PRESS) {
    glm::mat4 temp;
    temp[0][0] = -1;
    temp[1][1] = -1;
    temp[2][2] = 1;
    temp[3][2] = 4;
    m_view_transform = temp;
    updateView();
  }
  else if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
    shader = "planet";
    uploadUniforms();
  }
  else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
    shader = "cel";
    uploadUniforms();
  }
}

// handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
  //mouse handling
  // rotate around x-axis (up and down)
  m_view_transform = glm::rotate(m_view_transform, float(pos_y*0.005), glm::vec3{ -1.0f, 0.0f, 0.0f });
  // calculate up vector from model to world space to avoid 6DOF
  glm::fvec4 world_up = glm::fvec4{ 0.0f, -1.0f, 0.0f, 1.0f } * m_view_transform;
  // rotate around world space y-axis (left and right)
  m_view_transform = glm::rotate(m_view_transform, float(pos_x*0.005), glm::vec3{ -world_up });
  updateView();
}

// load shader programs
void ApplicationSolar::initializeShaderPrograms() {
  // store shader program objects in container
  m_shaders.emplace("planet", shader_program{m_resource_path + "shaders/simple.vert",
                                           m_resource_path + "shaders/simple.frag"});
  m_shaders.emplace("star", shader_program{m_resource_path + "shaders/star.vert",
											m_resource_path + "shaders/star.frag"});
  m_shaders.emplace("orbit", shader_program{ m_resource_path + "shaders/orbit.vert",
                      m_resource_path + "shaders/orbit.frag" });
  m_shaders.emplace("cel", shader_program{ m_resource_path + "shaders/cel.vert",
                      m_resource_path + "shaders/cel.frag" });

  // request uniform locations for shader program
  m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
  m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
  m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
  m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
  m_shaders.at("planet").u_locs["Color"] = -1;

  m_shaders.at("star").u_locs["ViewMatrix"] = -1;
  m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;

  m_shaders.at("orbit").u_locs["ModelMatrix"] = -1;
  m_shaders.at("orbit").u_locs["ViewMatrix"] = -1;
  m_shaders.at("orbit").u_locs["ProjectionMatrix"] = -1;

  m_shaders.at("cel").u_locs["NormalMatrix"] = -1;
  m_shaders.at("cel").u_locs["ModelMatrix"] = -1;
  m_shaders.at("cel").u_locs["ViewMatrix"] = -1;
  m_shaders.at("cel").u_locs["ProjectionMatrix"] = -1;
  m_shaders.at("cel").u_locs["Color"] = -1;
}

// load models
void ApplicationSolar::initializeGeometry() {
  // initialize planet properties
  // size, disatnce, rotation speed, orbital speed, orbit, color
  planets.at(0) = std::make_shared<Planet>(0.400, 0.00, 1, 0.00, false, glm::fvec3{1.0, 0.40, 0.00});		// Sun
  planets.at(1) = std::make_shared<Planet>(0.050, 0.55, 1, 3.74, true, glm::fvec3{0.40, 0.40, 0.40});		// Mercury
  planets.at(1)->parent = planets.at(0);
  planets.at(2) = std::make_shared<Planet>(0.080, 0.78, 1, 2.50, true, glm::fvec3{0.87, 0.85, 0.74});		// Venus
  planets.at(2)->parent = planets.at(0);
  planets.at(3) = std::make_shared<Planet>(0.090, 0.98, 1, 1.98, true, glm::fvec3{0.00, 0.20, 0.80});		// Earth
  planets.at(3)->parent = planets.at(0);
  planets.at(4) = std::make_shared<Planet>(0.070, 1.30, 1, 1.40, true, glm::fvec3{0.80, 0.00, 0.00});		// Mars
  planets.at(4)->parent = planets.at(0);
  planets.at(5) = std::make_shared<Planet>(0.140, 2.00, 1, 1.31, true, glm::fvec3{1.00, 0.80, 0.40});		// Jupiter
  planets.at(5)->parent = planets.at(0);
  planets.at(6) = std::make_shared<Planet>(0.120, 4.00, 1, 0.97, true, glm::fvec3{1.00, 0.89, 0.62});		// Saturn
  planets.at(6)->parent = planets.at(0);
  planets.at(7) = std::make_shared<Planet>(0.091, 8.00, 1, 0.68, true, glm::fvec3{0.60, 0.80, 1.00});		// Uranus 
  planets.at(7)->parent = planets.at(0);
  planets.at(8) = std::make_shared<Planet>(0.089, 11.0, 1, 0.54, true, glm::fvec3{0.40, 0.60, 1.00});		// Neptun
  planets.at(8)->parent = planets.at(0);
  planets.at(9) = std::make_shared<Planet>(0.030, 12.0, 1, 0.47, true, glm::fvec3{0.80, 0.53, 0.00});		// Pluto
  planets.at(9)->parent = planets.at(0);
  // dist is distance to earth, same applies to orb_speed
  // orbit of moon coming soon ;)
  planets.at(10) = std::make_shared<Planet>(0.035, 0.15, 0.08, 6, false, glm::fvec3{0.5, 0.5, 0.50});		// MOON
  planets.at(10)->parent = planets.at(3);

  model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

  // generate vertex array object
  glGenVertexArrays(1, &planet_object.vertex_AO);

  // bind the array for attaching buffers
  glBindVertexArray(planet_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &planet_object.vertex_BO);

  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planet_model.data.size(), planet_model.data.data(), GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::POSITION]);
  // activate second attribute on gpu
  glEnableVertexAttribArray(1);
  // second attribute is 3 floats with no offset & stride
  glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::NORMAL]);

   // generate generic buffer
  glGenBuffers(1, &planet_object.element_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet_object.element_BO);
  // configure currently bound array buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * planet_model.indices.size(), planet_model.indices.data(), GL_STATIC_DRAW);

  // store type of primitive to draw
  planet_object.draw_mode = GL_TRIANGLES;
  // transfer number of indices to model object 
  planet_object.num_elements = GLsizei(planet_model.indices.size());

  // stars

  // C++11 style randomizer
  std::random_device rand;
  std::uniform_real_distribution<float> format(-100, 100);
  // star array
  // for (int i = 0; i < stars.size(); i++) {
  //   stars.at(i) = glm::vec3{format(rand), format(rand), format(rand)};
  // }

  std::vector<GLfloat> stars;
  for (int i = 0; i < 1000; i++) {
    // set random coordinates between -100 and 100 for xyz
    glm::fvec3 pos{ format(rand), format(rand), format(rand) };

    // push xyz coordinates to stars vector
    stars.push_back(pos[0]);   // x coordinates
    stars.push_back(pos[1]);   // y coordinates
    stars.push_back(pos[2]);   // z coordinates

    // normalize xyz coordinates
    glm::normalize(pos);
    // and push them as color
  
    stars.push_back(pos[0]);   // red value
    stars.push_back(pos[1]);   // green value
    stars.push_back(pos[2]);   // blue value
    
    // -> rainbow-colored gradient
    // -> indicates position
    // -> better orientation for user
    // -> more realistic: rgb(1.0, 1.0, 0.8)
    // better solution (not used due to assignment):
    // in shader star.vert copy position to out_color
  }

  glGenBuffers(1, &star_object.vertex_BO);
  glBindBuffer(GL_ARRAY_BUFFER, star_object.vertex_BO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * stars.size(), stars.data(), GL_STATIC_DRAW);

  glGenVertexArrays(1, &star_object.vertex_AO);
  glBindVertexArray(star_object.vertex_AO);
  
  glBindBuffer(GL_ARRAY_BUFFER, star_object.vertex_BO);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, 0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(float)*6, (void *)(sizeof(float)*3));

  star_object.draw_mode = GL_POINTS;
  star_object.num_elements = GLsizei(stars.size()/6);

  // Orbit
  // create orbit model and vertices
  std::vector<float> orbit = create_orbit(64);

  glGenBuffers(1, &orbit_object.vertex_BO);
  glBindBuffer(GL_ARRAY_BUFFER, orbit_object.vertex_BO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * orbit.size(), orbit.data(), GL_STATIC_DRAW);

  glGenVertexArrays(1, &orbit_object.vertex_AO);
  glBindVertexArray(orbit_object.vertex_AO);

  glBindBuffer(GL_ARRAY_BUFFER, orbit_object.vertex_BO);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(float) * 3, 0);

  // draw mode is GL_LINE_LOOP to connect single vertices with edges
  orbit_object.draw_mode = GL_LINE_LOOP;
  orbit_object.num_elements = GLsizei(orbit.size()/3);
}

void ApplicationSolar::upload_planet_transforms(Planet const& planet) const {
  glm::mat4 model_matrix;
  std::stack<glm::mat4> matrices;
  std::shared_ptr<Planet> current = planet.parent;
  while (current != nullptr) {
    model_matrix = glm::rotate(glm::fmat4{}, float(/*glfwGetTime() */ current->orbital_speed), glm::fvec3{ 0.0f, 1.0f, 0.0f });
    model_matrix = glm::translate(model_matrix, glm::fvec3{ 0.0f, 0.0f, -1.0f * current->distance });
    matrices.push(model_matrix);
    current = current->parent;
  }
  model_matrix = glm::mat4{};
  while (!matrices.empty()) {
    model_matrix *= matrices.top();
    matrices.pop();
  }

	// bind shader to upload uniforms
	glUseProgram(m_shaders.at(shader).handle);

	model_matrix = glm::rotate(model_matrix, float(/*glfwGetTime() */ planet.orbital_speed), glm::fvec3{ 0.0f, 1.0f, 0.0f });
	model_matrix = glm::translate(model_matrix, glm::fvec3{ 0.0f, 0.0f, -1.0f * planet.distance });
	model_matrix = glm::scale(model_matrix, glm::fvec3{planet.size});
	glUniformMatrix4fv(m_shaders.at(shader).u_locs.at("ModelMatrix"),
		1, GL_FALSE, glm::value_ptr(model_matrix));
	
	// extra matrix for normal transformation to keep them orthogonal to surface
  glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
	glUniformMatrix4fv(m_shaders.at(shader).u_locs.at("NormalMatrix"),
		1, GL_FALSE, glm::value_ptr(normal_matrix));

	glm::fvec3 color_vector = planet.color;
	glUniform3fv(m_shaders.at(shader).u_locs.at("Color"), 1, glm::value_ptr(color_vector));

	// bind the VAO to draw
	glBindVertexArray(planet_object.vertex_AO);

	// draw bound vertex array using bound shader
	glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);

  if (planet.orbit) {
    upload_orbit(planet);
  }
}

void ApplicationSolar::upload_stars() const {
  // bind the VAO to draw
  glBindVertexArray(star_object.vertex_AO);
  glUseProgram(m_shaders.at("star").handle);

  // draw bound vertex array using bound shader
  glDrawArrays(star_object.draw_mode, NULL, star_object.num_elements);
}

// create vector containing orbit points
std::vector<float> ApplicationSolar::create_orbit(int const depth) const {
  std::vector<float> orbit; // | x y z | x y z | ...
  float interval = (atan(1) * 8) / depth;  // 0->360 = 0->2PI = 0->atan(1)*8
  for (int i = 0; i < depth; i++) {
    orbit.push_back(cos(i*interval));
    orbit.push_back(0.0);
    orbit.push_back(sin(i*interval));
  }
  return orbit;
}

void ApplicationSolar::upload_orbit(Planet const& planet) const {
  glBindVertexArray(orbit_object.vertex_AO);
  glUseProgram(m_shaders.at("orbit").handle);

  glm::mat4 model_matrix;
  model_matrix = glm::scale(model_matrix, glm::fvec3{ planet.distance });
  glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ModelMatrix"),
    1, GL_FALSE, glm::value_ptr(model_matrix));

  glDrawArrays(orbit_object.draw_mode, NULL, orbit_object.num_elements);
}

ApplicationSolar::~ApplicationSolar() {
  glDeleteBuffers(1, &planet_object.vertex_BO);
  glDeleteBuffers(1, &planet_object.element_BO);
  glDeleteBuffers(1, &star_object.vertex_BO);
  glDeleteBuffers(1, &star_object.element_BO);
  glDeleteBuffers(1, &orbit_object.vertex_BO);
  glDeleteBuffers(1, &orbit_object.element_BO);
  glDeleteVertexArrays(1, &planet_object.vertex_AO);
  glDeleteVertexArrays(1, &star_object.vertex_AO);
  glDeleteVertexArrays(1, &orbit_object.vertex_AO);
}

// exe entry point
int main(int argc, char* argv[]) {
  Launcher::run<ApplicationSolar>(argc, argv);
}
