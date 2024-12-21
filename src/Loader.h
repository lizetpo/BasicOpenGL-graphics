#ifndef LOADER_H
#define LOADER_H

#include <string>
#include "Scene.h"

class Loader {
public:
    static void loadScene(const std::string &filename, Scene &scene);
};

#endif // LOADER_H
