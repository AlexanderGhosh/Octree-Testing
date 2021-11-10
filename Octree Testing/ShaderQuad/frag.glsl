#version 440 core

out vec4 colour;

in vec2 texCoords;
uniform sampler2D tex;



void main()
{
    vec2 dim = textureSize(tex, 0);

	vec2 rcpRes = vec2(1.0) / dim.xy;
	vec3 col = vec3(0.0);
	int numSamples = 4;
	float rcpNumSamples = 1.0 / float(numSamples);
	for(int x = 0; x < numSamples; ++x)
	{
		for(int y = 0; y < numSamples; ++y)
		{
			vec2 adj = vec2(float(x), float(y));
			vec2 uv = (gl_FragCoord.xy + adj * rcpNumSamples) * rcpRes;
			col += texture(tex, uv).rgb;
		}
	}
	col /= float(numSamples * numSamples);

	colour = vec4(col, 1);
}