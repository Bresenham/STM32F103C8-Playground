//
// Created by Standardbenutzer on 31.03.2018.
//

#include "renderer.h"
#include "Structs/Sphere.h"
#include "Structs/camera.h"

struct Sphere spheres[] = {
        { .center = {.x = 0.0f, .y = -0.0f, .z = 3.0f}, .color = {.r = 255, .g = 255, .b = 255}, .radius = 4.0f, .isEmitter = true },
        { .center = {.x = 0.0f, .y = 8.0f, .z = 3.0f}, .color = {.r = 0, .g = 0, .b = 255}, .radius = 4.0f, .isEmitter = false },
        { .center = {.x = 0.0f, .y = -8.0f, .z = 3.0f}, .color = {.r = 255, .g = 0, .b = 0}, .radius = 4.0f, .isEmitter = false }
};

uint16_t buffer[W * H];
uint8_t samples = 0;
const struct camera c = {.pos = {.x = 0.0, .y = .25f, .z = 50.25}};

void display() {
  int i = 0;
  for(uint8_t y = 0; y < H; y++) {
    for(uint8_t x = 0; x < W; x++) {
      drawPixel(x + (128 - W) / 2, y + (128 - H) / 2, buffer[i]);
      i++;
    }
  }
}

void render() {
    samples++;
    uint16_t i = 0;
    for (uint8_t y = 0; y < H; y++) {
        for (uint8_t x = 0; x < W; x++) {
            const struct Ray ray = generateRay(c, x, y, W, H, samples);
            const struct RGB color = trace(ray, 0);

            uint16_t color_code;
            
            const uint8_t r = color.r;
            const uint8_t g = color.g;
            const uint8_t b = color.b;
            
            color_code = (r & 0x1f) << 11;
            color_code |= (g & 0x3f) << 5;
            color_code |= (b & 0x1f) << 0;
            
            buffer[i] += color_code;

            i++;
        }
    }

    /*
    i = 0;
    for(uint8_t y = 0; y < H; y++) {
      for(uint8_t x = 0; x < W; x++) {
        buffer[i * 3 + 0] /= samples;
        buffer[i * 3 + 1] /= samples;
        buffer[i * 3 + 2] /= samples;
        i++;
      }
    }
    */
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

    r /= 255.0;
    g /= 255.0;
    b /= 255.0;

    return (struct RGB){ .r = r, .g = g, .b = b};
}