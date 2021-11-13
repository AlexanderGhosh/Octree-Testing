#version 450 core

layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_out;

uniform vec2 dim;
uniform float seed;
float seed_ = seed / dot(gl_GlobalInvocationID, gl_GlobalInvocationID);

const int DIFFUSE = 0;
const int METAL = 1;

const vec3 BG_COLOUR = mix(vec3(1), vec3(0, 0.25, 0.75),
  gl_GlobalInvocationID.y / dim.y);

const int MAX_CHILDREN = 10;
const int OBJECT_COUNT = 2;
const float GAMMA_VALUE = 1.0 / 2.2;
const int MAX_SAMPLES = 4;
const float INV_SAMPLES = 1.0 / float(MAX_SAMPLES);
const vec3 CAMERA_POS = vec3(0, 0, -5);
const float INFINITY = 1000000000.0;
const float EPSLON = 0.001;

struct Circle {
  vec3 pos;
  float radius;
  vec3 col;
  int material;
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
  int material;
};

Circle objects[OBJECT_COUNT];

Ray CreateRay(vec3 origin = vec3(0), vec3 dir = vec3(0, 0, -1));
Circle CreateCircle(vec3 pos=vec3(0), 
  float radius=1, vec3 colour=vec3(1, 0, 0), int mat = DIFFUSE);
HitInfo CreateHitInfo(bool hit = false, vec3 hitPos = vec3(0),
  vec3 colour = BG_COLOUR, vec3 norm = vec3(0, 1, 0), float dist = 0, int mat = DIFFUSE);

HitInfo GetHitInfo(Circle obj, Ray r, vec2 range = vec2(0, INFINITY));

HitInfo GetHitInfo(Ray ray);

vec3 GammaCorrect(vec3 col){
  return pow(col, vec3(GAMMA_VALUE));
}

Ray CreateRay(vec2 pixelCoords){
  // Image
  float aspect_ratio = dim.x / dim.y;
  int image_width = int(dim.x);
  int image_height = int(dim.y);

  // Camera
  float viewport_height = 2.0;
  float viewport_width = aspect_ratio * viewport_height;
  float focal_length = 1.0;

  vec3 horizontal = vec3(viewport_width, 0, 0);
  vec3 vertical = vec3(0, viewport_height, 0);
  vec3 lower_left_corner = CAMERA_POS - horizontal * 0.5 - 
    vertical * 0.5 - vec3(0, 0, focal_length);

  // Create Ray
  float u = pixelCoords.x / float(image_width - 1);
  float v = pixelCoords.y / float(image_height - 1);
  vec3 dir = lower_left_corner + u * horizontal + v * vertical - CAMERA_POS;
  dir = normalize(dir);

  Ray ray = CreateRay(CAMERA_POS, dir);
  return ray;
}

float ReScale(float value, float a, float b, float i, float j);
vec2 ReScale(vec2 value, float a, float b, float i, float j);
vec3 ReScale(vec3 value, float a, float b, float i, float j);


// Material calculations
vec3 DiffuseRays(in int childrenRays, in Ray startingRay);


float Rand();
vec2 Rand2();
vec3 Rand3();
vec3 RandSphere();
vec3 RandHemisphere(vec3 normal);

void main(){
  // Objects
  objects[0] = CreateCircle(vec3(0, 0, -10), 2, vec3(0.75));
  objects[1] = CreateCircle(vec3(0, -102, -10), 100, vec3(0.5));

  vec3 pixel = vec3(0, 0, 0);
  
  ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

  // Test objects

  // initalise values
  Ray ray = CreateRay();
  HitInfo hit = CreateHitInfo();
  vec3 col = vec3(0);
  float weight = 1;
  for(int i = 0; i < MAX_SAMPLES; i++){
    // antialiasing
    vec2 coords = vec2(pixel_coords);
    vec2 offset = ReScale(Rand2(), 0, 1, -1, 1);
    //coords += offset;
    ray = CreateRay(coords);
    // diffuse rays
    /*for(int j = 0; j < MAX_CHILDREN; j++){
      weight *= 0.5;
      hit = GetHitInfo(ray);
      if(!hit.hit){
        // hit the backgound
        col = BG_COLOUR;
        break;
      }
      // create new ray
      vec3 target = vec3(0);
      target = hit.normal + RandHemisphere(hit.normal);
      target = normalize(target);
      ray = CreateRay(hit.hitPos, target);
      //col = ReScale(target, -1, 1, 0, 1);
      //break;
    }p*/
    //pixel = col * weight;
    pixel += DiffuseRays(MAX_CHILDREN, ray);
    //pixel = col;
    // reset values
    hit = CreateHitInfo();
    col = vec3(0);
    weight = 1;
  }

  pixel *= INV_SAMPLES;

  pixel = GammaCorrect(pixel);

  if(pixel.x < EPSLON){
    pixel = vec3(1, 1, 0);
  }
  
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
  float radius=1, vec3 colour=vec3(1, 0, 0), int mat = DIFFUSE){
    
  Circle obj;
  obj.pos = pos;
  obj.radius = radius;
  obj.col = colour;
  obj.material = mat;
  return obj;
}

HitInfo CreateHitInfo(bool hit = false, vec3 hitPos = vec3(0),
  vec3 colour = BG_COLOUR, vec3 norm = vec3(0), float dist = 0, int mat = DIFFUSE) {
  HitInfo info;
  info.hit = hit;
  info.hitPos = hitPos;
  info.colour = colour;
  info.normal = norm;
  info.dist = dist;
  return info;
}

HitInfo GetHitInfo(Circle obj, Ray r, vec2 range = vec2(0, INFINITY)){
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
    float inv_a = 1.0 / a;
    float sqrt_d = sqrt(discrim);
    // Find the nearest root that lies in the acceptable range.
    dist = (-half_b - sqrt_d) * inv_a;
    if (dist < range.x || range.y < dist) {
        dist = (-half_b + sqrt_d) * inv_a;
        if (dist < range.x || range.y < dist){
          dist = -1;
        }
    }
  }

  HitInfo info;
  info.dist = dist;

  if(dist > 0){
    vec3 p = r.origin + r.dir * dist;
    info.hitPos = p;
    info.normal = normalize(p - obj.pos);
    info.colour = obj.col;
    info.hit = true;
    info.material = obj.material;
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
  seed_ = fract(sin(seed_) * 10000.0);
  return res;
}

vec2 Rand2() {
  return vec2(Rand(), Rand());
}

vec3 Rand3() {
  return vec3(Rand(), Rand(), Rand());
}

vec3 RandSphere() {
  int counter = 0;
  const int max_trys = 10;
  vec3 res = vec3(0);
  float d = 0;
  while (counter++ < max_trys){
    res = Rand3();
    res = ReScale(res, 0, 1, -1, 1);
    d = dot(res, res);
    if(d <= 1.0){
      break;
    }
  }
  return res;
  /*vec3 p = Rand3();
  p = ReScale(p, 0, 1, -1, 1);
  p = normalize(p);
  return p;*/
}

vec3 RandHemisphere(vec3 normal){
  vec3 s = normalize(RandSphere());
  if(dot(s, normal) > 0.0){
    return s;
  }
  else{
    return -s;
  }
  return s;
}

HitInfo GetHitInfo(Ray ray){HitInfo res = CreateHitInfo();
  HitInfo info = CreateHitInfo();
  for(int i = 0; i < OBJECT_COUNT; i++){
    Circle obj = objects[i];
    info = GetHitInfo(obj, ray, vec2(EPSLON, INFINITY));
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

vec3 DiffuseRays(in int childrenRays, in Ray startingRay){
  float weight = 1.0;
  Ray ray = startingRay;
  HitInfo hit = CreateHitInfo();
  vec3 col = vec3(0);
  for(int j = 0; j < childrenRays; j++){
    weight *= 0.5;
    hit = GetHitInfo(ray);
    if(!hit.hit){
      // hit the backgound
      col = BG_COLOUR;
      break;
    }
    // create new ray
    vec3 target = vec3(0);
    target = hit.normal + RandHemisphere(hit.normal);
    target = normalize(target);
    ray = CreateRay(hit.hitPos, target);
  }
  return col * weight;
}