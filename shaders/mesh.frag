#version 430 core

out vec4 color;

in vec3 N;
in vec3 V;
in vec3 L;

// Material properties
uniform float b = 0.5; 		//k_blue
uniform float y = 0.5;		//k_yellow
uniform float alpha = 0.3; 		//k_cool
uniform float beta = 0.1;		//k_warm
uniform vec3 objectColor = vec3(0.7,  0.2,  0.9); //kd
uniform vec3 specular_albedo = vec3(1.0); //white

uniform float kd = 0.5;
uniform float ks = 0.6;
uniform float ke = 5.0;

void main(void)
{    	

    // Calculate R locally
    vec3 R = reflect(-L, N);
    
    //calculate k_cool and k_warm
    float LDotN = dot(N, L);
    vec3 k_blue = vec3(0, 0, b);
    vec3 k_yellow = vec3(y, y, 0);
    vec3 k_cool = k_blue + alpha * objectColor;
    vec3 k_warm = k_yellow + beta * objectColor;
    
    vec3 diffuse = (( 1 - LDotN)/2) * k_cool + (1 - (1 - LDotN)/2) * k_warm;
    vec3 specular = ks * pow(max(dot(R, V), 0.0), ke) * specular_albedo;

    color = vec4(diffuse + specular, 1.0);
}
