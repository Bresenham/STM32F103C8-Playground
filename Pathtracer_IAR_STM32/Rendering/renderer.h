//
// Created by Standardbenutzer on 31.03.2018.
//

#ifndef PATHTRACER_RENDERER_H
#define PATHTRACER_RENDERER_H

#include "Structs/Ray.h"
#include "../ILI9163C.h"

#define W 38
#define H 38
#define TRACEDEPTH  3
#define SAMPLES 1

void render();
void display();
struct RGB trace(struct Ray ry, int tdepth);

#endif //PATHTRACER_RENDERER_H
