#version 430

layout(location=0) in  mediump vec2 vTexCoord;
layout(location=0) out vec4 fragColor;

layout(set=0, binding=1, std140) uniform FragData
{
  vec4  uColor;
  vec3  uCustomPosition;
};

layout( set=0, binding=2 ) uniform  sampler2D sTexture;

void main()
{
  if( texture( sTexture, vTexCoord ).a <= 0.0001 )
  {
    discard;
  }

  vec2 wrapTexCoord = vec2( ( vTexCoord.x / 4.0 ) + ( uCustomPosition.x / 4.0 ) + ( uCustomPosition.z / 2.0 ), vTexCoord.y / 4.0 );
  vec4 color = texture( sTexture, wrapTexCoord );
  float positionWeight = ( uCustomPosition.y + 0.3 ) * color.r * 2.0;

  fragColor = vec4( positionWeight, positionWeight, positionWeight, 0.9 ) * uColor + vec4( uColor.xyz, 0.0 );
}
