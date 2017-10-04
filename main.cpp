#include <iostream>
#include <memory>
#include "sphere.h"
#include "hitablelist.h"
#include "float.h"
#include "camera.hpp"
#include "material.hpp"

int inBall = 0;
int outBall = 0;
float hit_sphere(const vec3& center, float radius, const ray& r){
  vec3 oc = r.origin() - center;
  float a = dot(r.direction(), r.direction());
  float b = 2.0 * dot(oc, r.direction());
  float c = dot(oc, oc) - radius * radius;
  float discriminant = b*b - 4*a*c;
  if(discriminant < 0){
    return -1.0;
  }
  else{
    return ( -b - sqrt(discriminant)) / (2.0*a);
  }
}

vec3 color(const ray& r){
  float t = hit_sphere(vec3(0, 0, -1), 0.5, r);
  if (t > 0.0){
    inBall += 1;
    vec3 N = unit_vector(r.point_at_parameter(t) - vec3(0, 0, -1)); // normal vector at hit point
    return 0.5 * vec3(N.x() + 1, N.y() +1, N.z() + 1);
  }
  else{
    outBall +=1;
    vec3 unit_direction = unit_vector(r.direction());
    t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
  }

}

vec3 color(const ray& r, hitable* world){
  hit_record rec;
  if(world->hit(r, 0.001, MAXFLOAT, rec)){
    vec3 target = rec.p + rec.normal + random_in_unit_sphere();
    return 0.5*color( ray(rec.p, target - rec.p), world);
    // return 0.5*vec3(rec.normal.x()+1, rec.normal.y() + 1, rec.normal.z()+1);
  }
  else{
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 -t )*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
  }
}

vec3 color(const ray& r, hitable* world, int depth){
  hit_record rec;
  // if(world->hit(r, 0.0, MAXFLOAT, rec)){
  // use 0.001 instead of 0.0 get rid of the shadow acne problem
  if(world->hit(r, 0.001, MAXFLOAT, rec)){
    ray scattered;
    vec3 attenuation;
    if(depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)){
      return attenuation*color(scattered, world, depth+1);
    }
    else {
      return vec3(0, 0, 0);
    }
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

  hitable *list[5];
  list[0] = new sphere(vec3(0, 0, -1), 0.5, std::make_shared<lambertian>(lambertian(vec3(0.8, 0.3, 0.3))));
  list[1] = new sphere(vec3(0, -100.5, -1), 100, std::make_shared<lambertian>(lambertian(vec3(0.8, 0.8, 0.0))));
  list[2] = new sphere(vec3(1, 0, -1), 0.5, std::make_shared<metal>(metal(vec3(0.8, 0.6, 0.2), 0.3)));
  list[3] = new sphere(vec3(-1, 0, -1), 0.5, std::make_shared<dielectric>(dielectric(1.5)));
  list[4] = new sphere(vec3(-1, 0, -1), -0.45, std::make_shared<dielectric>(dielectric(1.5)));

  // hitable *list[2];
  // float R = cos(M_PI/4);
  // list[0] = new sphere(vec3(-R, 0, -1), R, std::make_shared<lambertian>(lambertian(vec3(0, 0, 1))));
  // list[1] = new sphere(vec3(R,0, -1), R, std::make_shared<lambertian>(lambertian(vec3(1, 0, 0))));

  hitable *world = new hitable_list(list, 5);

  // camera cam(90, float(nx)/ float(ny));
  camera cam(vec3(-2, 2, 1), vec3(0, 0, -1), vec3(0, 1, 0), 90, float(nx)/float(ny));
  for(int j=ny-1; j>=0; j--){
    for(int i=0 ; i<nx; i++){
      vec3 col(0, 0, 0);
      // ray r(origin, lower_left_corner + u*horizontal+ v*vertical);
      for(int s = 0; s < ns; s++){
        float u = float(i + drand48()) / float(nx);
        float v = float(j + drand48()) / float(ny);
        ray r = cam.get_ray(u, v);
        vec3 p = r.point_at_parameter(2.0);
        col += color(r, world, 0);
      }
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
