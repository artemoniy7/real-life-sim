#include <GLFW/glfw3.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

namespace {
constexpr int WindowWidth = 1280;
constexpr int WindowHeight = 720;
constexpr float FieldOfViewDegrees = 75.0F;
constexpr float NearPlane = 0.05F;
constexpr float FarPlane = 250.0F;
constexpr float PlayerEyeHeight = 1.75F;
constexpr float PlayerMoveSpeed = 5.0F;
constexpr float MouseSensitivityDegrees = 0.12F;
constexpr float MinPitchDegrees = -85.0F;
constexpr float MaxPitchDegrees = 85.0F;
constexpr float GrassPlatformHalfSize = 40.0F;
constexpr float GrassPlatformY = 0.0F;
constexpr float Pi = 3.14159265358979323846F;

struct Vec3 {
  float x = 0.0F;
  float y = 0.0F;
  float z = 0.0F;
};

Vec3 operator+(const Vec3 &left, const Vec3 &right) {
  return {left.x + right.x, left.y + right.y, left.z + right.z};
}

Vec3 operator-(const Vec3 &left, const Vec3 &right) {
  return {left.x - right.x, left.y - right.y, left.z - right.z};
}

Vec3 operator*(const Vec3 &value, float scale) {
  return {value.x * scale, value.y * scale, value.z * scale};
}

Vec3 &operator+=(Vec3 &left, const Vec3 &right) {
  left = left + right;
  return left;
}

float dot(const Vec3 &left, const Vec3 &right) {
  return left.x * right.x + left.y * right.y + left.z * right.z;
}

Vec3 cross(const Vec3 &left, const Vec3 &right) {
  return {left.y * right.z - left.z * right.y,
          left.z * right.x - left.x * right.z,
          left.x * right.y - left.y * right.x};
}

float length(const Vec3 &value) { return std::sqrt(dot(value, value)); }

Vec3 normalize(const Vec3 &value) {
  const float valueLength = length(value);
  if (valueLength <= 0.00001F) {
    return {};
  }
  return value * (1.0F / valueLength);
}

float radians(float degrees) { return degrees * Pi / 180.0F; }

struct Camera {
  Vec3 position{0.0F, PlayerEyeHeight, 6.0F};
  float yawDegrees = 0.0F;
  float pitchDegrees = 0.0F;

  [[nodiscard]] Vec3 forward() const {
    const float yaw = radians(yawDegrees);
    const float pitch = radians(pitchDegrees);
    const float horizontal = std::cos(pitch);
    return normalize({std::sin(yaw) * horizontal, std::sin(pitch),
                      -std::cos(yaw) * horizontal});
  }

  [[nodiscard]] Vec3 flatForward() const {
    const float yaw = radians(yawDegrees);
    return normalize({std::sin(yaw), 0.0F, -std::cos(yaw)});
  }

  [[nodiscard]] Vec3 right() const {
    return normalize(cross(flatForward(), {0.0F, 1.0F, 0.0F}));
  }

  void rotate(float yawDeltaDegrees, float pitchDeltaDegrees) {
    yawDegrees += yawDeltaDegrees;
    pitchDegrees = std::clamp(pitchDegrees + pitchDeltaDegrees, MinPitchDegrees,
                              MaxPitchDegrees);
  }
};

enum class PlayerAnimationState { Idle, Walk };

struct AnimationController {
  PlayerAnimationState state = PlayerAnimationState::Idle;
  PlayerAnimationState previousState = PlayerAnimationState::Idle;
  float time = 0.0F;
  float blendTime = 0.0F;
  float blendDuration = 0.16F;

  void setState(PlayerAnimationState nextState) {
    if (state == nextState) {
      return;
    }

    previousState = state;
    state = nextState;
    time = 0.0F;
    blendTime = 0.0F;
  }

  void update(bool isMoving, float deltaTime) {
    setState(isMoving ? PlayerAnimationState::Walk
                      : PlayerAnimationState::Idle);
    time += deltaTime;
    blendTime = std::min(blendTime + deltaTime, blendDuration);
  }

  [[nodiscard]] float blendFactor() const {
    if (blendDuration <= 0.00001F) {
      return 1.0F;
    }
    const float progress = std::clamp(blendTime / blendDuration, 0.0F, 1.0F);
    return progress * progress * (3.0F - 2.0F * progress);
  }

  [[nodiscard]] std::string debugName() const {
    return state == PlayerAnimationState::Walk ? "Walk" : "Idle";
  }
};

struct Player {
  Vec3 feetPosition{0.0F, GrassPlatformY, 6.0F};
  AnimationController animation;
};

struct InputState {
  bool hasPreviousMousePosition = false;
  double previousMouseX = 0.0;
  double previousMouseY = 0.0;
};

void errorCallback(int error, const char *description) {
  std::cerr << "GLFW error " << error << ": " << description << '\n';
}

void framebufferSizeCallback(GLFWwindow *, int width, int height) {
  glViewport(0, 0, width, height);
}

void multiplyMatrix(const float matrix[16]) { glMultMatrixf(matrix); }

void loadPerspective(float fieldOfViewDegrees, float aspectRatio,
                     float nearPlane, float farPlane) {
  const float top = nearPlane * std::tan(radians(fieldOfViewDegrees) * 0.5F);
  const float right = top * aspectRatio;

  const float matrix[16] = {nearPlane / right,
                            0.0F,
                            0.0F,
                            0.0F,
                            0.0F,
                            nearPlane / top,
                            0.0F,
                            0.0F,
                            0.0F,
                            0.0F,
                            -(farPlane + nearPlane) / (farPlane - nearPlane),
                            -1.0F,
                            0.0F,
                            0.0F,
                            -(2.0F * farPlane * nearPlane) /
                                (farPlane - nearPlane),
                            0.0F};

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  multiplyMatrix(matrix);
}

void loadViewMatrix(const Camera &camera) {
  const Vec3 forward = camera.forward();
  const Vec3 right = camera.right();
  const Vec3 up = cross(right, forward);

  const float matrix[16] = {right.x,
                            up.x,
                            -forward.x,
                            0.0F,
                            right.y,
                            up.y,
                            -forward.y,
                            0.0F,
                            right.z,
                            up.z,
                            -forward.z,
                            0.0F,
                            -dot(right, camera.position),
                            -dot(up, camera.position),
                            dot(forward, camera.position),
                            1.0F};

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  multiplyMatrix(matrix);
}

void updateMouseLook(GLFWwindow *window, InputState &input, Camera &camera) {
  double mouseX = 0.0;
  double mouseY = 0.0;
  glfwGetCursorPos(window, &mouseX, &mouseY);

  if (!input.hasPreviousMousePosition) {
    input.previousMouseX = mouseX;
    input.previousMouseY = mouseY;
    input.hasPreviousMousePosition = true;
    return;
  }

  const double deltaX = mouseX - input.previousMouseX;
  const double deltaY = mouseY - input.previousMouseY;
  input.previousMouseX = mouseX;
  input.previousMouseY = mouseY;

  camera.rotate(static_cast<float>(deltaX) * MouseSensitivityDegrees,
                static_cast<float>(-deltaY) * MouseSensitivityDegrees);
}

Vec3 readMovement(GLFWwindow *window, const Camera &camera) {
  Vec3 movement{};
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    movement += camera.flatForward();
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    movement += camera.flatForward() * -1.0F;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    movement += camera.right();
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    movement += camera.right() * -1.0F;
  }
  return movement;
}

void clampPlayerToGrassPlatform(Player &player) {
  player.feetPosition.x =
      std::clamp(player.feetPosition.x, -GrassPlatformHalfSize + 0.2F,
                 GrassPlatformHalfSize - 0.2F);
  player.feetPosition.z =
      std::clamp(player.feetPosition.z, -GrassPlatformHalfSize + 0.2F,
                 GrassPlatformHalfSize - 0.2F);
  player.feetPosition.y = GrassPlatformY;
}

void updatePlayer(GLFWwindow *window, Player &player, Camera &camera,
                  float deltaTime) {
  const Vec3 movement = readMovement(window, camera);
  const bool isMoving = length(movement) > 0.0F;
  if (isMoving) {
    player.feetPosition += normalize(movement) * (PlayerMoveSpeed * deltaTime);
    clampPlayerToGrassPlatform(player);
  }

  player.animation.update(isMoving, deltaTime);
  camera.position = player.feetPosition + Vec3{0.0F, PlayerEyeHeight, 0.0F};
}

void drawGrassPlatform() {
  glColor3f(0.18F, 0.62F, 0.18F);
  glBegin(GL_QUADS);
  glVertex3f(-GrassPlatformHalfSize, GrassPlatformY, -GrassPlatformHalfSize);
  glVertex3f(GrassPlatformHalfSize, GrassPlatformY, -GrassPlatformHalfSize);
  glVertex3f(GrassPlatformHalfSize, GrassPlatformY, GrassPlatformHalfSize);
  glVertex3f(-GrassPlatformHalfSize, GrassPlatformY, GrassPlatformHalfSize);
  glEnd();

  glColor3f(0.10F, 0.38F, 0.10F);
  glBegin(GL_LINES);
  for (int line = -40; line <= 40; ++line) {
    glVertex3f(static_cast<float>(line), GrassPlatformY + 0.002F,
               -GrassPlatformHalfSize);
    glVertex3f(static_cast<float>(line), GrassPlatformY + 0.002F,
               GrassPlatformHalfSize);
    glVertex3f(-GrassPlatformHalfSize, GrassPlatformY + 0.002F,
               static_cast<float>(line));
    glVertex3f(GrassPlatformHalfSize, GrassPlatformY + 0.002F,
               static_cast<float>(line));
  }
  glEnd();
}

void updateWindowTitle(GLFWwindow *window, const Player &player) {
  const std::string title =
      "Simple First Person Engine - Animation: " + player.animation.debugName();
  glfwSetWindowTitle(window, title.c_str());
}

void renderScene(const Camera &camera, int framebufferWidth,
                 int framebufferHeight) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  const float aspectRatio = framebufferHeight > 0
                                ? static_cast<float>(framebufferWidth) /
                                      static_cast<float>(framebufferHeight)
                                : 1.0F;
  loadPerspective(FieldOfViewDegrees, aspectRatio, NearPlane, FarPlane);
  loadViewMatrix(camera);

  drawGrassPlatform();
}

void configureOpenGl() {
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glClearColor(0.50F, 0.74F, 1.0F, 1.0F);
}

GLFWwindow *createWindow() {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  GLFWwindow *window =
      glfwCreateWindow(WindowWidth, WindowHeight, "Simple First Person Engine",
                       nullptr, nullptr);
  if (window == nullptr) {
    std::cerr << "Failed to create a GLFW window.\n";
    return nullptr;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  return window;
}
} // namespace

int main() {
  glfwSetErrorCallback(errorCallback);
  if (glfwInit() != GLFW_TRUE) {
    std::cerr << "Failed to initialize GLFW.\n";
    return EXIT_FAILURE;
  }

  GLFWwindow *window = createWindow();
  if (window == nullptr) {
    glfwTerminate();
    return EXIT_FAILURE;
  }

  configureOpenGl();
  InputState input;
  Player player;
  Camera camera;
  camera.position = player.feetPosition + Vec3{0.0F, PlayerEyeHeight, 0.0F};

  float previousTime = static_cast<float>(glfwGetTime());
  while (glfwWindowShouldClose(window) == GLFW_FALSE) {
    const float currentTime = static_cast<float>(glfwGetTime());
    const float deltaTime = currentTime - previousTime;
    previousTime = currentTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    updateMouseLook(window, input, camera);
    updatePlayer(window, player, camera, deltaTime);
    updateWindowTitle(window, player);

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    renderScene(camera, framebufferWidth, framebufferHeight);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return EXIT_SUCCESS;
}
