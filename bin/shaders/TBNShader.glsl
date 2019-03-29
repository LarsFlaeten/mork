#ifdef _VERTEX_

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec3 aTang;
layout (location = 3) in vec3 aBitang;
layout (location = 4) in vec2 aUv;

out VS_OUT {
    vec3 norm;
    vec3 tang;
    vec3 bitang;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normalMat;
void main()
{
   gl_Position = projection * view * model * vec4(aPos, 1.0);
   vs_out.norm = vec3(projection * view * vec4(normalMat * aNorm, 0.0));
   vs_out.tang = vec3(projection * view * vec4(normalMat * aTang, 0.0));
   vs_out.bitang = vec3(projection * view * vec4(normalMat * aBitang, 0.0));
   //vs_out.norm = vec3(projection * view * model * vec4(aNorm, 0.0));
   //vs_out.tang = vec3(projection * view * model * vec4(aTang, 0.0));
   //vs_out.bitang = vec3(projection * view * model * vec4(aBitang, 0.0));




}

#endif

#ifdef _GEOMETRY_ 

layout (triangles) in;
layout (line_strip, max_vertices = 18) out;
uniform float scale;

in VS_OUT {
  vec3 norm;
  vec3 tang;
  vec3 bitang;
} gs_in[];

out vec3 color;

void generateTBN(int index) {
    color = vec3(1.0, 0.0, 0.0);
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].tang, 0.0)*scale;
    EmitVertex();
    EndPrimitive();

    color = vec3(0.0, 1.0, 0.0);
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].bitang, 0.0)*scale;
    EmitVertex();
    EndPrimitive();

    color = vec3(0.0, 0.0, 1.0);
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].norm, 0.0)*scale;
    EmitVertex();
    EndPrimitive();


}

void main()
{
   generateTBN(0);
   generateTBN(1);
   generateTBN(2);
};

#endif

#ifdef _FRAGMENT_
in vec3 color;
out vec4 FragColor;

void main()
{
   FragColor = vec4(color, 1.0);
};

#endif
