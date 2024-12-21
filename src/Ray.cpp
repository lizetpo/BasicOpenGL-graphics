#include "Ray.h"

Ray::Ray(const glm::vec3 &o, const glm::vec3 &d) : origin(o), direction(glm::normalize(d)) {}
