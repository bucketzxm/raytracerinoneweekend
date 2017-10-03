#ifndef HITABLEH
#define HITABLEH

#include "ray.h"
class material;

class hit_record {
public:
  float t;
  vec3 p;
  vec3 normal;
  std::shared_ptr<material> mat_ptr;
};

class hitable{

public:
  virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;

};


#endif
