#ifdef _VERTEX_
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aUv;
out vec2 texCoord;

void main()
{
   gl_Position = vec4(aPos, 1.0);
   texCoord = aUv;
};

#endif

#ifdef _FRAGMENT_

out vec4 FragColor;
in vec2 texCoord;

uniform sampler2D tex;

void main()
{
   vec4 color = texture(tex, texCoord);
   FragColor = vec4(color.rgba);
};

#endif
