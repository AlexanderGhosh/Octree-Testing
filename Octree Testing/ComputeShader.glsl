#version 450 core

layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_out;

uniform vec2 dim;
uniform float seed;
float seed_ = seed / dot(gl_GlobalInvocationID, gl_GlobalInvocationID);

const vec3 BG_COLOUR = mix(vec3(1), vec3(0, 0.25, 0.75),
  gl_GlobalInvocationID.y / dim.y);

const int MAX_CHILDREN = 10;
const int OBJECT_COUNT = 2;
const float GAMMA_VALUE = 1.0 / 2.2;

struct Circle {
  vec3 pos;
  float radius;
  vec3 col;
};
struct Ray{
  vec3 origin;
  vec3 dir;
};
struct HitInfo{
  vec3 hitPos;
  vec3 colour;
  vec3 normal;
  float dist;
  bool hit;
};

Circle objects[OBJECT_COUNT];

Ray CreateRay(vec3 origin = vec3(0), vec3 dir = vec3(0, 0, -1));
Circle CreateCircle(vec3 pos=vec3(0), 
  float radius=1, vec3 colour=vec3(1, 0, 0));
HitInfo CreateHitInfo(bool hit = false, vec3 hitPos = vec3(0),
  vec3 colour = BG_COLOUR, vec3 norm = vec3(0, 1, 0), float dist = 0);

HitInfo GetHitInfo(Circle obj, Ray r);

HitInfo GetHitInfo(Ray ray);

vec3 GammaCorrect(vec3 col){
  return pow(col, vec3(GAMMA_VALUE));
}


float ReScale(float value, float a, float b, float i, float j);
vec2 ReScale(vec2 value, float a, float b, float i, float j);
vec3 ReScale(vec3 value, float a, float b, float i, float j);


float Rand();
vec3 Rand3();
vec3 RandSphere();

void main(){
  // Objects
  objects[0] = CreateCircle(vec3(0, 0, -10), 2, vec3(0.75));
  objects[1] = CreateCircle(vec3(0, -102, -10), 100, vec3(0.5));

  // Image
  float aspect_ratio = dim.x / dim.y;
  int image_width = int(dim.x);
  int image_height = int(dim.y);

  // Camera
  float viewport_height = 2.0;
  float viewport_width = aspect_ratio * viewport_height;
  float focal_length = 1.0;

  vec3 origin = vec3(0, 0, -6);
  vec3 horizontal = vec3(viewport_width, 0, 0);
  vec3 vertical = vec3(0, viewport_height, 0);
  vec3 lower_left_corner = origin - horizontal * 0.5 - 
    vertical * 0.5 - vec3(0, 0, focal_length);

  vec3 pixel = vec3(0, 0, 0);
  
  ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

  // Create Ray
  float u = float(pixel_coords.x) / (image_width - 1);
  float v = float(pixel_coords.y) / (image_height - 1);
  vec3 dir = lower_left_corner + u * horizontal + v * vertical - origin;
  dir = normalize(dir);

  vec3 col = vec3(0);

  /*for(int i = 0; i < MAX_CHILDREN; i++){
    ray = CreateRay(origin, dir);

    o1 = GetHitInfo(obj1, ray);
    o2 = GetHitInfo(obj2, ray);

    vec3 c = vec3(0);

    if(o1.hit && !o2.hit){
      c = o1.colour;
      vec3 target = o1.normal + RandSphere();
      ray = CreateRay(o1.hitPos, normalize(target));
    }
    else if(!o1.hit && o2.hit){
      c = o2.colour;
      vec3 target = o2.normal + RandSphere();
      ray = CreateRay(o2.hitPos, normalize(target));
    }
    else if(!o1.hit && !o2.hit){
      // hit the background
      col = o1.colour;
      break;
    }
    else{
      // both hit
      if(o1.dist < o2.dist){
        c = o1.colour;
        vec3 target = o1.normal + RandSphere();
        ray = CreateRay(o1.hitPos, normalize(target));
      }
      else{
        c = o2.colour;
        vec3 target = o2.normal + RandSphere();
        ray = CreateRay(o2.hitPos, normalize(target));
      }
    }
    col = c;
    weight *= 0.5;
  }
  */

  // Test objects

  Ray ray = CreateRay(origin, dir);
  HitInfo hit = CreateHitInfo();
  float weight = 1;
  for(int i = 0; i < MAX_CHILDREN; i++){
    weight *= 0.5;
    hit = GetHitInfo(ray);
    if(!hit.hit){
      // hit the backgound
      col = BG_COLOUR;
      break;
    }
    // create new ray
    vec3 target = vec3(0);
    target = hit.normal + RandSphere();
    target = normalize(target);
    ray = CreateRay(hit.hitPos, target);
  }
  col *= weight;

  ray = CreateRay(origin, dir);
  hit = GetHitInfo(ray);

  pixel = hit.colour;
  pixel = col;

  pixel = GammaCorrect(pixel);

  imageStore(img_out, pixel_coords, vec4(pixel, 1));
}

float ReScale(float value, float a, float b, float i, float j){
  float or = (b - a);  
  float nr = (j - i);  
  return (((value - a) * nr) / or) + i;
}

vec2 ReScale(vec2 value, float a, float b, float i, float j){
  vec2 or = (vec2(b) - vec2(a));  
  vec2 nr = (vec2(j) - vec2(i));  
  return (((value - a) * nr) / or) + i;
}

vec3 ReScale(vec3 value, float a, float b, float i, float j){
  vec3 or = (vec3(b) - vec3(a));  
  vec3 nr = (vec3(j) - vec3(i));  
  return (((value - a) * nr) / or) + i;
}

Ray CreateRay(vec3 origin = vec3(0), vec3 dir = vec3(0, 0, -1)){
  Ray r;
  r.origin = origin;
  r.dir = dir;
  return r;
}

Circle CreateCircle(vec3 pos=vec3(0), 
  float radius=1, vec3 colour=vec3(1, 0, 0)){
    
  Circle obj;
  obj.pos = pos;
  obj.radius = radius;
  obj.col = colour;
  return obj;
}

HitInfo CreateHitInfo(bool hit = false, vec3 hitPos = vec3(0),
  vec3 colour = BG_COLOUR, vec3 norm = vec3(0), float dist = 0) {
  HitInfo info;
  info.hit = hit;
  info.hitPos = hitPos;
  info.colour = colour;
  info.normal = norm;
  info.dist = dist;
  return info;
}

HitInfo GetHitInfo(Circle obj, Ray r){
  // Test Hit
  vec3 oc = r.origin - obj.pos;
  float a = dot(r.dir, r.dir);
  float half_b = dot(oc, r.dir);
  float c = dot(oc, oc) - obj.radius * obj.radius;
  float discrim = half_b * half_b - a * c;

  float dist = 0;

  if(discrim < 0){
    // No roots
    dist = -1;
  }
  else{
    // Two or more roots
    dist = (-half_b - sqrt(discrim)) / a;
  }

  HitInfo info;
  info.dist = dist;

  if(dist > 0){
    vec3 p = r.origin + r.dir * dist;
    info.hitPos = p;
    info.normal = normalize(p - obj.pos);
    info.colour = obj.col;
    info.hit = true;
  }
  else{
    info.hitPos = vec3(0);
    info.normal = vec3(0);
    info.colour = BG_COLOUR;
    info.hit = false;
  }

  return info;
}

float Rand() {
  float res = fract(sin(seed_) * 1000000.0);
  seed_ += res;
  return res;
}

vec3 Rand3() {
  return vec3(Rand(), Rand(), Rand());
}

vec3 RandSphere() {
  vec3 p = Rand3();
  p = ReScale(p, 0, 1, -1, 1);
  p = normalize(p);
  return p;
}

HitInfo GetHitInfo(Ray ray){
  HitInfo res = CreateHitInfo();
  HitInfo info = CreateHitInfo();
  for(int i = 0; i < OBJECT_COUNT; i++){
    Circle obj = objects[i];
    info = GetHitInfo(obj, ray);
    if(info.hit && res.hit){
      if(info.dist < res.dist){
        res = info;
      }
    } 
    else if(info.hit){
      res = info;
    }
  }
  return res;
}