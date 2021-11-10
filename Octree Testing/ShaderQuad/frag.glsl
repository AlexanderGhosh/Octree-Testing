#version 440 core

out vec4 colour;

in vec2 texCoords;
uniform sampler2D tex;
uniform float seed;
float seed_ = seed / dot(gl_FragCoord, gl_FragCoord);

float rand(){
  float res = fract(sin(seed_) * 100000000.0);
  seed_ += res;
  return res;
}

vec3 getColour(vec2 coords){
	return texture(tex, coords).rgb;
}

void main()
{
    vec2 dim = textureSize(tex, 0);
	vec2 coords = vec2(texCoords * 0.5 + 0.5);
	//coords = gl_FragCoord.xy;
	int samples = 4;

	vec3 pixel = vec3(0);

	/*for (int i = 0; i < samples; i++){
		float u = coords.x + rand();
		float v = coords.y + rand();

		u = u * 2 - 1;
		v = v * 2 - 1;

		vec3 c = getColour(vec2(u, v));
		// c = getColour(coords);
		pixel += c;
	}

	pixel /= (samples);

	colour = vec4(pixel, 1);*/
	colour = vec4(getColour(texCoords), 1);
}