#include "scene.h"


static std::vector<std::string> animationList;

#include <filesystem>
static std::vector<std::string> scan_animations(const char* path)
{
  std::vector<std::string> animations;
  for (auto& p : std::filesystem::recursive_directory_iterator(path))
  {
    auto filePath = p.path();
    if (p.is_regular_file() && filePath.extension() == ".fbx")
      animations.push_back(filePath.string());
  }
  return animations;
}



static glm::mat4 get_projective_matrix()
{
  const float fovY = 90.f * DegToRad;
  const float zNear = 0.01f;
  const float zFar = 500.f;
  return glm::perspective(fovY, engine::get_aspect_ratio(), zNear, zFar);
}

void application_init(Scene &scene)
{
  animationList = scan_animations("resources/Animations");

  // Set light
  scene.light.lightDirection = glm::normalize(glm::vec3(-1, -1, 0));
  scene.light.lightColor = glm::vec3(1.f);
  scene.light.ambient = glm::vec3(0.2f);

  // Set camera
  scene.userCamera.projection = get_projective_matrix();
  //engine::onWindowResizedEvent += [&](const std::pair<int, int> &) { scene.userCamera.projection = get_projective_matrix(); };

  ArcballCamera &cam = scene.userCamera.arcballCamera;
  cam.curZoom = cam.targetZoom = 0.5f;
  cam.maxdistance = 5.f;
  cam.distance = cam.curZoom * cam.maxdistance;
  cam.lerpStrength = 10.f;
  cam.mouseSensitivity = 0.5f;
  cam.wheelSensitivity = 0.05f;
  cam.targetPosition = glm::vec3(0.f, 1.f, 0.f);
  cam.targetRotation = cam.curRotation = glm::vec2(DegToRad * -90.f, DegToRad * -30.f);
  cam.rotationEnable = false;

  scene.userCamera.transform = calculate_transform(scene.userCamera.arcballCamera);

  // Add some mousekeyboard callbacks
  engine::onMouseButtonEvent += [&](const SDL_MouseButtonEvent &e) { arccam_mouse_click_handler(e, scene.userCamera.arcballCamera); };
  engine::onMouseMotionEvent += [&](const SDL_MouseMotionEvent &e) { arccam_mouse_move_handler(e, scene.userCamera.arcballCamera, scene.userCamera.transform); };
  engine::onMouseWheelEvent += [&](const SDL_MouseWheelEvent &e) { arccam_mouse_wheel_handler(e, scene.userCamera.arcballCamera); };

  engine::onKeyboardEvent += [](const SDL_KeyboardEvent &e) { if (e.keysym.sym == SDLK_F5 && e.state == SDL_RELEASED) recompile_all_shaders(); };

  // Create first model
  {
    ModelAsset motusMan = load_model("resources/MotusMan_v55/MotusMan_v55.fbx");

    auto material = make_material("character", "sources/shaders/character_vs.glsl", "sources/shaders/character_ps.glsl");
    material->set_property("mainTex", create_texture2d("resources/MotusMan_v55/MCG_diff.jpg"));

    scene.characters.emplace_back(Character{
      "MotusMan_v55",
      glm::identity<glm::mat4>(),
      motusMan.meshes,
      std::move(material)
    });
    scene.models.push_back(std::move(motusMan));
  }

  // Create second model
  {
    ModelAsset ruby = load_model("resources/sketchfab/ruby.fbx");

    auto whiteMaterial = make_material("character", "sources/shaders/character_vs.glsl", "sources/shaders/character_ps.glsl");
    const uint8_t whiteColor[4] = { 255, 255, 255, 255 };
    whiteMaterial->set_property("mainTex", create_texture2d(whiteColor, 1, 1, 4));

    scene.characters.emplace_back(Character{
      "Ruby",
      glm::translate(glm::identity<glm::mat4>(), glm::vec3(2.f, 0.f, 0.f)),
      ruby.meshes,
      std::move(whiteMaterial)
    });
    scene.models.push_back(std::move(ruby));
  }

  std::fflush(stdout);
}