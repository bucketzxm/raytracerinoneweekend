#include <iostream>

#include "sphere.h"
#include "hitablelist.h"
#include "float.h"
#include "camera.hpp"
// bool hit_sphere(const vec3& center, float radius, const ray& r){
 
//   if (discriminant < 0){
//     return -1.0;
//   }else{
//     return (-b - sqrt(discriminant)) / (2.0*a);
//   }
// }

vec3 random_in_unit_sphere(){
  vec3 p;
  do {
    p = 2.0*vec3(drand48(), drand48(), drand48()) - vec3(1, 1, 1);
  }while (p.squared_length() >= 1.0);
  return p;
}

vec3 color(const ray& r, hitable* world){
  hit_record rec;
  // if(world->hit(r, 0.0, MAXFLOAT, rec)){
  // use 0.001 instead of 0.0 get rid of the shadow acne problem
  if(world->hit(r, 0.001, MAXFLOAT, rec)){
    vec3 target = rec.p + rec.normal + random_in_unit_sphere();
    return 0.5*color(ray(rec.p, target-rec.p), world);
    // return 0.5*vec3(rec.normal.x() + 1, rec.normal.y()+1, rec.normal.z() +1);
  }
  else{
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t)*vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
  }
}

int main(int argc, char* argv[]){
  int nx = 200;
  int ny = 100;
  int ns = 100;
  std::cout<< "P3\n" << nx << " " << ny << "\n255\n";

  vec3 lower_left_corner(-2.0, -1.0, -1.0);
  vec3 horizontal(4.0, 0.0, 0.0);
  vec3 vertical(0.0, 2.0, 0.0);
  vec3 origin(0.0, 0.0, 0.0);

  hitable *list[2];
  list[0] = new sphere(vec3(0, 0, -1), 0.5);
  list[1] = new sphere(vec3(0, -100.5, -1), 100);
  hitable *world = new hitable_list(list, 2);
  camera cam;
  for(int j=ny-1; j>=0; j--){
    for(int i=0 ; i<nx; i++){
      vec3 col(0, 0, 0);
      float u = float(i) / float(nx);
      float v = float(j) / float(ny);
      // ray r = ray(origin, lower_left_corner+u*horizontal+v*vertical-origin);
      ray r = cam.get_ray(u, v);
      vec3 p = r.point_at_parameter(2.0);

      for(int s=0; s < ns; s++){
        col += color(r, world);
      }
      // col = color(r, world);
      col /= float(ns);

      // yields light grey
      col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
      int ir = int(255.99*col[0]);
      int ig = int(255.99*col[1]);
      int ib = int(255.99*col[2]);

      std::cout << ir << " " << ig << " " << ib << "\n";
    }
  }
}