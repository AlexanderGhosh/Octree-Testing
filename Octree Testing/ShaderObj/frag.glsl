#version 440 core
out vec4 colour;

float plot (vec2 st, float pct){
    return smoothstep( pct-0.01, pct, st.y) - smoothstep( pct, pct+0.01, st.y);
}

void main()
{
	vec2 st = gl_FragCoord.xy/vec2(100, 100);
    vec3 color = vec3(0.0);

    vec3 pct = vec3(st.x);

    // pct.r = smoothstep(0.0,1.0, st.x);
    // pct.g = sin(st.x*PI);
    // pct.b = pow(st.x,0.5);

    color = mix(vec3(1, 0, 0), vec3(0, 1, 0), pct);

    // Plot transition lines for each channel
    color = mix(color,vec3(1.0,0.0,0.0),plot(st,pct.r));
    color = mix(color,vec3(0.0,1.0,0.0),plot(st,pct.g));
    color = mix(color,vec3(0.0,0.0,1.0),plot(st,pct.b));
	colour = vec4(color, 1);
}