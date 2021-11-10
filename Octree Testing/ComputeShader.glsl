#version 450 core

layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_out;

uniform vec2 dim;
uniform float seed;
float seed_ = seed / dot(gl_GlobalInvocationID, gl_GlobalInvocationID);

struct Circle {
  vec3 pos;
  float radius;
  vec3 col;
};
struct Ray{
  vec3 origin;
  vec3 dir;
};

Circle obj1;
Circle obj2;

vec3 backColour = vec3(0, 0, 0);

vec3 getColour(Circle obj, Ray r){
  vec3 oc = r.origin - obj.pos;
  float a = dot(r.dir, r.dir);
  float b = 2 * dot(oc, r.dir);
  float c = dot(oc, oc) - obj.radius * obj.radius;
  float discrim = b * b - 4 * a * c;

  float t = 0;

  if(discrim < 0){
    t = -1;
  }
  else{
    t = (-b - sqrt(discrim)) / (2 * a);
  }

  vec3 n;
  if(t > 0){
    vec3 p = r.origin + r.dir * t;
    n = normalize(p + vec3(0, 0, 1));
    //return obj.col;
  }
  else{
    return backColour;
  }

  return (n + 1 * 0.5);
}

float rand(){
  float res = fract(sin(seed_) * 1000000.0);
  seed_ += res;
  return res;
}

vec3 rand3(){
  return vec3(rand(), rand(), rand());
}

void main(){
  // Object
  obj1.pos = vec3(0, 0, -10);
  obj1.radius = 2;
  obj1.col = vec3(1, 0, 0);

  obj2.pos = vec3(0, -105, 10);
  obj2.radius = 100;
  obj2.col = vec3(1, 1, 0);

  // Image
  float aspect_ratio = dim.x / dim.y;
  int image_width = int(dim.x);
  int image_height = int(dim.y);

  // Camera

  float viewport_height = 2.0;
  float viewport_width = aspect_ratio * viewport_height;
  float focal_length = 1.0;

  vec3 origin = vec3(0, 1, 0);
  vec3 horizontal = vec3(viewport_width, 0, 0);
  vec3 vertical = vec3(0, viewport_height, 0);
  vec3 lower_left_corner = origin - horizontal * 0.5 - 
    vertical * 0.5 - vec3(0, 0, focal_length);

  vec4 pixel = vec4(0, 0, 0, 1.0);
  
  ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

  float u = float(pixel_coords.x) / (image_width - 1);
  float v = float(pixel_coords.y) / (image_height - 1);

  vec3 dir = lower_left_corner + u * horizontal + v * vertical - origin;

  dir = normalize(dir);

  pixel.rgb = dir;

  Ray ray;
  ray.origin = origin;
  ray.dir = dir;

  pixel.rgb = getColour(obj1, ray);
  pixel.rgb = pixel.rgb == backColour ? getColour(obj2, ray) : pixel.rgb;

  imageStore(img_out, pixel_coords, pixel);
}