#version 430

layout(location=0) in vec2 vTexCoord;
layout( set=0, binding=1, std140) uniform FragData
{
  float uDelta;
};

layout(set=0, binding=2) uniform sampler2D sTexture;

layout(location=0) out vec4 fragColor;

void main()
{
  vec4 color = vec4(0.0);
  vec2 texCoord = vTexCoord * 2. - 1.;
  mat2 rotation = mat2(cos(uDelta), -sin(uDelta), sin(uDelta), cos(uDelta));
  texCoord = (rotation * texCoord) * .5 + .5;
  color += texture( sTexture, texCoord );
  fragColor = color;
}
