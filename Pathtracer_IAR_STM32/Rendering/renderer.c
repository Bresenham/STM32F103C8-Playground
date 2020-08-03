//
// Created by Standardbenutzer on 31.03.2018.
//

#include "stdio.h"
#include "renderer.h"
#include "Structs/Sphere.h"
#include "Structs/camera.h"

struct Sphere spheres[] = {
        { .center = {.x = 0.0f, .y = -0.0f, .z = 3.0f}, .color = {.r = 255, .g = 255, .b = 255}, .radius = 4.0f, .isEmitter = true },
        { .center = {.x = 0.0f, .y = 8.0f, .z = 3.0f}, .color = {.r = 255, .g = 255, .b = 255}, .radius = 4.0f, .isEmitter = false },
        { .center = {.x = 0.0f, .y = -8.0f, .z = 3.0f}, .color = {.r = 255, .g = 255, .b = 255}, .radius = 4.0f, .isEmitter = false }
};

uint32_t buffer[W * H * 3];
uint8_t samples = 0;
const struct camera c = {.pos = {.x = 0.0, .y = .25f, .z = 50.25f}};

void display() {
  uint16_t x,y,i = 0;
  for (y = 0; y < H; y++)
  {
      for (x = 0; x < W; x++)
      {
          const uint16_t r = buffer[i * 3 + 0] / samples;
          const uint16_t g = buffer[i * 3 + 1] / samples;
          const uint16_t b = buffer[i * 3 + 2] / samples;

          uint16_t color_code;
          color_code  = (r >> 3) << 11;
          color_code |= (g >> 2) << 5;
          color_code |= (b >> 3) << 0;
          
          drawPixel(x + (128 - W) / 2, y + (128 - H) / 2, color_code);
          i++;
          /*
          char buffer[50];
          sprintf(buffer, "Pixel %3d,%3d has color %3d,%3d,%3d\r\n", x, y, r, g, b);
          send_str(buffer, sizeof(buffer));
          */
      }
  }
}

void render() {
    samples++;
    uint16_t i = 0;
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            const struct Ray r = generateRay(c, x, y, W, H, samples);
            const struct RGB color = trace(r, 0);
            buffer[i * 3 + 0] += color.r;
            buffer[i * 3 + 1] += color.g;
            buffer[i * 3 + 2] += color.b;
            i++;
        }
    }
}

struct RGB trace(const struct Ray ry, int tdepth) {
    if(tdepth == TRACEDEPTH) return (struct RGB){.r = 0, .g = 0, .b = 0};

    float hitDistance  = 1e20f;
    struct Sphere hitObject;
    for(int i = 0; i < (sizeof(spheres) / sizeof(spheres[0])); i++) {
        float dist = intersectSphere(spheres[i], ry);
        if(dist != -1.0 && dist < hitDistance) {
            hitDistance = dist;
            hitObject = spheres[i];
        }
    }

    if(hitDistance == 1e20f) return (struct RGB){.r = 0, .g = 0, .b = 0};
    if(hitObject.isEmitter) return hitObject.color;

    const struct Point hitPoint = add(ry.origin, mult(ry.dir, hitDistance * 0.998));
    const struct Point nrml = sphereNormal(hitObject, hitPoint);
    struct Point rnd = diffuse();

    /* Wrong hemisphere */
    /*
    if(dot(rnd, nrml) < 0.0)
        rnd = mult(rnd, -1.0);
    */
    const struct Ray reflectionRay = (struct Ray){ .origin = hitPoint, .dir = norm(rnd) };

    struct RGB returnColor = trace(reflectionRay, tdepth + 1);
    int r = hitObject.color.r * returnColor.r;
    int g = hitObject.color.g * returnColor.g;
    int b = hitObject.color.b * returnColor.b;

    r /= 255;
    g /= 255;
    b /= 255;

    return (struct RGB){ .r = r, .g = g, .b = b};
}