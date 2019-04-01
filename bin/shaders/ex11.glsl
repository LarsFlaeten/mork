#ifdef _VERTEX_

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec3 aTang;
layout (location = 3) in vec3 aBitang;
layout (location = 4) in vec2 aUv;

out VS_OUT {
  vec2 texCoord;
  vec3 fragPos;
  mat3 TBN;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normalMat;

void main()
{
   gl_Position = projection * view * model * vec4(aPos, 1.0);
   vs_out.fragPos = vec3(model * vec4(aPos, 1.0));
   vs_out.texCoord = aUv;
   vec3 T = normalize(normalMat * aTang);
   vec3 B = normalize(normalMat * aBitang);
   vec3 N = normalize(normalMat * aNorm);
   vs_out.TBN = mat3(T, B, N);
};

#endif
#ifdef _FRAGMENT_

#include "shaders/materials.glhl"
#include "shaders/lights.glhl"

out vec4 FragColor;
in VS_OUT {
   vec2 texCoord;
   vec3 fragPos;
   mat3 TBN;
} vs_out;



uniform PointLight pointLight;
uniform DirLight dirLight;
uniform SpotLight spotLight;
uniform Material material;
uniform vec3 viewPos;

void main()
{
    // Calculate normal:
    vec3 normal = vec3(0,0,1);
    if(material.numNormalLayers>0u) {
        normal = evaluateTextureLayers(vec3(0), material.normalLayers, material.numNormalLayers, vs_out.texCoord);
        normal = normalize(normal*2.0 - 1.0);
    }
    normal = normalize(vs_out.TBN * normal);

    // Base color contribution:
    vec3 ambientColor = evaluateTextureLayers(material.ambientColor, material.ambientLayers, material.numAmbientLayers, vs_out.texCoord);
    vec3 diffuseColor = evaluateTextureLayers(material.diffuseColor, material.diffuseLayers, material.numDiffuseLayers, vs_out.texCoord);
    vec3 specularColor = evaluateTextureLayers(material.specularColor, material.specularLayers, material.numSpecularLayers, vs_out.texCoord);
    vec3 emissiveColor = evaluateTextureLayers(material.emissiveColor, material.emissiveLayers, material.numEmissiveLayers, vs_out.texCoord);

    // Calculate light contribution:
    vec3 lightResult = vec3(0.0, 0.0, 0.0);
    vec3 viewDir = normalize(viewPos - vs_out.fragPos);
    lightResult += CalcPointLight(ambientColor, diffuseColor, specularColor, pointLight, normal, vs_out.fragPos, viewDir, material);
    lightResult += CalcDirLight(ambientColor, diffuseColor, specularColor, dirLight, normal, viewDir, material);
    lightResult += CalcSpotLight(ambientColor, diffuseColor, specularColor, spotLight, normal, vs_out.fragPos, viewPos, material);


    vec3 total = emissiveColor + lightResult;
    
    FragColor = vec4(total, 1.0);
};


#endif
