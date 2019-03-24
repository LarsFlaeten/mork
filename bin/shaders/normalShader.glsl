#ifdef _VERTEX_

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aUv;

out VS_OUT {
  vec3 normal;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normalMat;
void main()
{
   gl_Position = projection * view * model * vec4(aPos, 1.0);
   vs_out.normal = vec3(projection * view * vec4(normalMat * aNorm, 0.0));
}

#endif

#ifdef _GEOMETRY_ 

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;
uniform float scale;

in VS_OUT {
  vec3 normal;
} gs_in[];

void generateNormal(int index) {
   // Only draw normals the are eheading towards us? (= Front face)
   if(gs_in[index].normal.z <= 0.0) {
       gl_Position = gl_in[index].gl_Position;
       EmitVertex();
       gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * scale;
       EmitVertex();
       EndPrimitive();
   }
}

void main()
{
   generateNormal(0);
   generateNormal(1);
   generateNormal(2);
};

#endif

#ifdef _FRAGMENT_

out vec4 FragColor;

void main()
{
   FragColor = vec4(1.0, 1.0, 0.0, 1.0);
};

#endif
