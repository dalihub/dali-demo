//@version 100

INPUT mediump vec3 vNormal;
INPUT mediump vec3 vFragPos;

struct Material
{
  mediump vec3 ambient;
  mediump vec3 diffuse;
  mediump vec3 specular;
  mediump float shininess;
};

struct Light
{
  mediump vec3 position;
  mediump vec3 color;
};

UNIFORM_BLOCK CustomFrag
{
UNIFORM mediump vec3 viewPos; // custom uniform
UNIFORM Material material; // custom uniform
UNIFORM Light light;       // custom uniform
};

void main()
{
  // Ambient
  mediump vec3 ambient = material.ambient * light.color;
  // Diffuse
  mediump vec3 norm = normalize(vNormal);
  mediump vec3 lightDir = normalize(light.position - vFragPos);
  mediump float diff = max(dot(norm, lightDir), 0.0);
  mediump vec3 diffuse = material.diffuse * diff * light.color;

  // Specular
  mediump vec3 viewDir = normalize(viewPos - vFragPos);
  mediump vec3 reflectDir = reflect(-lightDir, norm);
  mediump float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  mediump vec3 specular = material.specular * spec * light.color;
  mediump vec3 result = (ambient + diffuse + specular);
  gl_FragColor = vec4(result, 1.0);
}
