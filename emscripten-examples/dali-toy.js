/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/*global  */
/* eslint-env browser */
/* eslint "brace-style": [2, "1tbs"] */
/* eslint "no-console": 0 */
/* eslint "no-underscore-dangle": 0 */

//
// Dali browser application for testing the javascript API.
//

// browser variables defined for eslint
var $;
var ace;
var dali;
// var canvas;
var app;
var eventHandler;
var assert;

//
// Global state
//
var test;
var database;

var uiApp;
var uiJavascriptTab;
var uiImageTab;
var uiShaderTab;


var actorIdToShaderSet = {};    // actor id to shader
var compiledShaders = {};       // compiled shaders by name

var animationList = [];         // list of animations that have been added [ { animation: new dali.Animation(), properties: [] } ]
var animationSelectionIndex;    // selected animation

//
//
// Demo Shaders
//
//
var shaderSourceSelection = 0;
var shaderSources = [
  {
    name: "pass through texture",
    hints: "",
    vertex:
    "\n" +
      "attribute mediump vec3 aPosition;\n" +
      "attribute mediump vec2 aTexCoord;\n" +
      "varying mediump vec2 vTexCoord;\n" +
      "uniform mediump vec3 uSize;\n" +
      "uniform mediump mat4 uModelView;\n" +
      "uniform mediump mat4 uProjection;\n" +
      "uniform mediump mat4 uMvpMatrix;\n" +
      "\n" +
      "void main(void)\n" +
      "{\n" +
      "  mediump vec4 vertexPosition = vec4(aPosition, 1.0);\n" +
      "  vertexPosition.xyz *= uSize;\n" +
      "  gl_Position = uMvpMatrix * vertexPosition;\n" +
      "  vTexCoord = aTexCoord;\n" +
      "}\n",
    fragment: "precision mediump float;\n" +
      "\n" +
      "uniform sampler2D sTexture;\n" +
      "uniform mediump vec4 uColor;\n" +
      "varying mediump vec2 vTexCoord;\n" +
      "\n" +
      "void main()\n" +
      "{\n" +
      "  gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n" +
      "}\n"
  },
  {
    name: "pass through color",
    hints: "",
    vertex: "\n" +
      "attribute mediump vec3 aPosition;\n" +
      "attribute mediump vec2 aTexCoord;\n" +
      "varying mediump vec2 vTexCoord;\n" +
      "uniform mediump vec3 uSize;\n" +
      "uniform mediump mat4 uModelView;\n" +
      "uniform mediump mat4 uProjection;\n" +
      "\n" +
      "void main(void)\n" +
      "{\n" +
      "  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n" +
      "  vertexPosition.xyz *= uSize;\n" +
      "  vTexCoord = aTexCoord;\n" +
      "}\n",
    fragment: "precision mediump float;\n" +
      "\n" +
      "uniform mediump vec4 uColor;\n" +
      "varying mediump vec2 vTexCoord;\n" +
      "\n" +
      "void main()\n" +
      "{\n" +
      "  gl_FragColor = vColor * uColor;\n" +
      "}\n"
  },
  {
    name: "sRGB Correction",
    hints: "",
    vertex: "\n" +
      "uniform mediump mat4 uModelView;\n" +
      "uniform mediump mat4 uProjection;\n" +
      "uniform mediump mat4 uModelMatrix;\n" +
      "attribute mediump vec3 aPosition;\n" +
      "attribute mediump vec2 aTexCoord;\n" +
      "varying mediump vec2 vTexCoord;\n" +
      "uniform mediump vec3 uSize;\n" +
      "\n" +
      "void main(void)\n" +
      "{\n" +
      "  mediump vec3 vertexPosition = aPosition * uSize;\n" +
      "  gl_Position = uProjection * uModelView * vec4(vertexPosition, 1.0);\n" +
      "  vTexCoord = aTexCoord;\n" +
      "}\n",
    fragment: "precision mediump float;\n" +
      "\n" +
      "uniform mediump vec4 uColor;\n" +
      "uniform sampler2D sTexture;\n" +
      "varying mediump vec2 vTexCoord;\n" +
      "\n" +
      "void main()\n" +
      "{\n" +
      "vec4 col = texture2D( sTexture, vTexCoord ) * uColor;\n" +
      "vec3 mask = vec3(greaterThan(col.rgb, vec3(0.0031308)));\n" +
      "vec3 o = mix(col.rgb * 12.92, \n" +
      "             pow(col.rgb, vec3(1.0/2.4)) * 1.055 - 0.055, \n" +
      "             mask);\n" +
      "gl_FragColor = vec4(o.r, o.g, o.b, col.a);\n" +
      "}\n"
  },
  {
    name: "adjust brightness",
    hints: "",
    animateTo: [ ["uGain", 1.0, "Linear", 0, 3],
                 ["uOffset", 0.2, "Linear", 0, 3] ],
    vertex:
    "\n" +
      "uniform mediump mat4 uModelView;\n" +
      "uniform mediump mat4 uProjection;\n" +
      "attribute mediump vec3 aPosition;\n" +
      "attribute mediump vec2 aTexCoord;\n" +
      "varying mediump vec2 vTexCoord;\n" +
      "uniform mediump vec3 uSize;\n" +
      "\n" +
      "void main(void)\n" +
      "{\n" +
      "  mediump vec3 vertexPosition = aPosition * uSize;\n" +
      "  gl_Position = uProjection * uModelView * vec4(vertexPosition, 1.0);\n" +
      "  vTexCoord = aTexCoord;\n" +
      "}\n" +
      "\n" +
      "",
    fragment:
    "precision mediump float;\n" +
      "\n" +
      "uniform mediump vec4 uColor;\n" +
      "uniform sampler2D sTexture;\n" +
      "varying mediump vec2 vTexCoord;\n" +
      "\n" +
      "uniform float uGain; // {default:1.0} \n" +
      "uniform float uOffset; // {default:0.0} \n" +
      "void main()\n" +
      "{\n" +
      "    vec4 t = texture2D( sTexture, vTexCoord );\n" +
      "    \n" +
      "    float y = 0.0 + (0.299 *t.r) + 0.587*t.g + 0.114*t.b;\n" +
      "    float cb= 128.0-(0.168736*t.r)-0.331264*t.g+0.5*t.b;\n" +
      "    float cr= 128.0+(0.5*t.r)-(0.418688*t.g)-0.081312*t.b;\n" +
      "    \n" +
      "    y = (y*uGain) + uOffset;\n" +
      "    \n" +
      "    vec4 col = vec4(t.a);\n" +
      "    \n" +
      "    col.r = y + 1.402*(cr-128.0);\n" +
      "    col.g = y - 0.34414*(cb-128.0)-0.71414*(cr-128.0);\n" +
      "    col.b = y + 1.722*(cb-128.0);\n" +
      "    \n" +
      "    \n" +
      "    gl_FragColor = col * uColor;\n" +
      "}\n" +
      "\n" +
      ""
  },
  {
    name: "wavey",
    hints: "",
    animateTo: [ ["uAmplitude", 0.2, "Linear", 0, 3],
                        ["uFrequency", 4, "Linear", 0, 3] ],
    vertex:  "\n" +
      "uniform mediump mat4 uModelView;\n" +
      "uniform mediump mat4 uProjection;\n" +
      "attribute mediump vec3 aPosition;\n" +
      "attribute mediump vec2 aTexCoord;\n" +
      "varying mediump vec2 vTexCoord;\n" +
      "uniform mediump vec3 uSize;\n" +
      "\n" +
      "void main(void)\n" +
      "{\n" +
      "  mediump vec3 vertexPosition = aPosition * uSize;\n" +
      "  gl_Position = uProjection * uModelView * vec4(vertexPosition, 1.0);\n" +
      "  vTexCoord = aTexCoord;\n" +
      "}\n",
    fragment: "precision mediump float;\n" +
      "\n" +
      "uniform mediump vec4 uColor;\n" +
      "uniform sampler2D sTexture;\n" +
      "varying mediump vec2 vTexCoord;\n" +
      "\n" +
      "uniform float uAmplitude;  //  {default:0.4, description:\"amplitude in x\"} \n" +
      "uniform float uFrequency;  // {default: 4, description:\"frequence in y\"} \n" +
      "void main()\n" +
      "{\n" +
      "  vec2 uv = vTexCoord.xy;\n" +
      "\n" +
      "  uv.x += sin(uv.y * 3.14 * uFrequency) * uAmplitude;\n" +
      "  \n" +
      "  vec4 color = texture2D(sTexture, uv);\n" +
      "  \n" +
      "  gl_FragColor = color;\n" +
      "}\n"
  },
  {
    name: "melt",
    hints: "",
    animateTo: [ ["uFactor", -4.0, "Linear", 0, 3] ],
    vertex:
    "\n" +
      "uniform mediump mat4 uModelView;\n" +
      "uniform mediump mat4 uProjection;\n" +
      "attribute mediump vec3 aPosition;\n" +
      "attribute mediump vec2 aTexCoord;\n" +
      "varying mediump vec2 vTexCoord;\n" +
      "uniform mediump vec3 uSize;\n" +
      "\n" +
      "void main(void)\n" +
      "{\n" +
      "  mediump vec3 vertexPosition = aPosition * uSize;\n" +
      "  gl_Position = uProjection * uModelView * vec4(vertexPosition, 1.0);\n" +
      "  vTexCoord = aTexCoord;\n" +
      "}\n" +
      "\n" +
      "",
    fragment:
    "precision mediump float;\n" +
      "\n" +
      "uniform mediump vec4 uColor;\n" +
      "uniform sampler2D sTexture;\n" +
      "varying mediump vec2 vTexCoord;\n" +
      "\n" +
      "uniform float uFactor; // {default:0.2, description:\"drip factor\"} \n" +
      "\n" +
      "    \n" +
      "float random( vec2 p )\n" +
      "{\n" +
      "    float q = p.x * 269.5 + p.y * 183.3;\n" +
      "  return fract( sin( q ) * 43758.5453 );\n" +
      "}\n" +
      "\n" +
      "void main()\n" +
      "{\n" +
      "  vec2 uv = vTexCoord.xy;\n" +
      "  \n" +
      "  float kindaRandom = (texture2D(sTexture, vec2(uv.x,0.5)).r + texture2D(sTexture, vec2(0.5,uv.x)).r) / 2.0;\n" +
      "  \n" +
      "  //kindaRandom = random( vec2(texture2D(sTexture, vec2(uv.x,0.5)).r, texture2D(sTexture, vec2(0.5,uv.y)).g) );\n" +
      "  \n" +
      "  uv.y +=  uFactor * kindaRandom;\n" +
      "  \n" +
      "\n" +
      "  gl_FragColor = texture2D( sTexture, uv ) * uColor;\n" +
      "}\n" +
      "\n" +
      "\n" +
      ""
  },
  {
    name: "round window",
    hints: "",
    animateTo: [ ["uRadius", 0.8, "Linear", 0, 3] ],
    vertex: "\n" +
      "uniform mediump mat4 uModelView;\n" +
      "uniform mediump mat4 uProjection;\n" +
      "attribute mediump vec3 aPosition;\n" +
      "attribute mediump vec2 aTexCoord;\n" +
      "varying mediump vec2 vTexCoord;\n" +
      "\n" +
      "void main(void)\n" +
      "{\n" +
      "  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n" +
      "  vTexCoord = aTexCoord;\n" +
      "}\n",
    fragment: "precision lowp float;\n" +
      "\n" +
      "uniform mediump vec4 uColor;\n" +
      "uniform sampler2D sTexture;\n" +
      "\n" +
      "uniform lowp float uRadius; // {default: 0.2}  \n" +
      "varying mediump vec2 vTexCoord;\n" +
      "void main()\n" +
      "{\n" +
      "  precision lowp float;\n" +
      "  lowp vec2 pos= vec2(vTexCoord.x-0.5,vTexCoord.y-0.5);\n" +
      "  lowp float radius = dot(pos, pos ) ;\n" +
      "  if( radius > (uRadius*uRadius) )\n" +
      "    discard;\n" +
      "  gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor ;\n" +
      "}\n"
  },
  {
    name: "mosaic",
    hints: "",
    animateTo: [ ["uPixelFactor", 0.3, "Linear", 0, 3] ],
    vertex: "\n" +
      "uniform mediump mat4 uModelView;\n" +
      "uniform mediump mat4 uProjection;\n" +
      "attribute mediump vec3 aPosition;\n" +
      "attribute mediump vec2 aTexCoord;\n" +
      "varying mediump vec2 vTexCoord;\n" +
      "uniform mediump vec3 uSize;\n" +
      "\n" +
      "void main(void)\n" +
      "{\n" +
      "  mediump vec3 vertexPosition = aPosition * uSize;\n" +
      "  gl_Position = uProjection * uModelView * vec4(vertexPosition, 1.0);\n" +
      "  vTexCoord = aTexCoord;\n" +
      "}\n",
    fragment: "precision mediump float;\n" +
      "\n" +
      "uniform mediump vec4 uColor;\n" +
      "uniform sampler2D sTexture;\n" +
      "varying mediump vec2 vTexCoord;\n" +
      "\n" +
      "  \n" +
      "uniform float uPixelFactor; //  {default:0.1, min:0.0, max:0.3} \n" +
      "\n" +
      "\n" +
      "float smooth(float f) {\n" +
      "  return 32.0*f*f*(0.25*f*f-0.5*f+0.25)+0.5;\n" +
      "}\n" +
      "\n" +
      "void main()\n" +
      "{\n" +
      "  vec2 resolution = vec2(1,1); // uniform vec2 resolution;\n" +
      "  //uPixelFactor = 8.0 + 8.0 * (0.5 + 0.5 * sin(globaltime * 0.25));\n" +
      "  vec2 chunkCoord = floor(vTexCoord.xy / uPixelFactor) * uPixelFactor;\n" +
      "  vec2 locCoord = (vTexCoord.xy - chunkCoord) / uPixelFactor;\n" +
      "  vec4 color = vec4(floor(5.0 * texture2D(sTexture, chunkCoord / resolution.xy).xyz) / 5.0, 1.0);\n" +
      "  float grey = (color.x + color.y + color.z) / 3.0;\n" +
      "  gl_FragColor = color * smooth(locCoord.x) * smooth(locCoord.y);\n" +
      "}\n"
  },
  {
    name: "burn",
    hints: "",
    animateTo: [ ["uThresh", 0.8, "Linear", 0, 3] ],
    vertex:
    "\n" +
      "uniform mediump mat4 uModelView;\n" +
      "uniform mediump mat4 uProjection;\n" +
      "attribute mediump vec3 aPosition;\n" +
      "attribute mediump vec2 aTexCoord;\n" +
      "varying mediump vec2 vTexCoord;\n" +
      "uniform mediump vec3 uSize;\n" +
      "\n" +
      "void main(void)\n" +
      "{\n" +
      "  mediump vec3 vertexPosition = aPosition * uSize;\n" +
      "  gl_Position = uProjection * uModelView * vec4(vertexPosition, 1.0);\n" +
      "  vTexCoord = aTexCoord;\n" +
      "}\n" +
      "\n" +
      "\n" +
      "\n" +
      "",
    fragment:
    "precision mediump float;\n" +
      "\n" +
      "uniform mediump vec4 uColor;\n" +
      "uniform sampler2D sTexture;\n" +
      "varying mediump vec2 vTexCoord;\n" +
      "\n" +
      "\n" +
      "float uScale = 2.0;  // {default:10.0, description:\"noise texture scaling\"} \n" +
      "uniform float uThresh;  // {default:1.1, description:\"threshold of noise for burn\"} \n" +
      "\n" +
      "float random( vec2 p )\n" +
      "{\n" +
      "    float q = p.x * 269.5 + p.y * 183.3;\n" +
      "  return fract( sin( q ) * 43758.5453 );\n" +
      "}\n" +
      "\n" +
      "float noise( vec2 point )\n" +
      "{\n" +
      "  vec2 p = floor( point );\n" +
      "  vec2 f = fract( point );\n" +
      "  return mix(\n" +
      "    mix( random( p + vec2( 0.0, 0.0 ) ), random( p + vec2( 1.0, 0.0 ) ), f.x ),\n" +
      "    mix( random( p + vec2( 0.0, 1.0 ) ), random( p + vec2( 1.0, 1.0 ) ), f.x ),\n" +
      "    f.y\n" +
      "  );\n" +
      "}\n" +
      "\n" +
      "float fractal( vec2 point )\n" +
      "{\n" +
      "    float sum = 0.0;\n" +
      "    float scale = 0.5;\n" +
      "    for ( int i = 0; i <  5; i++ )\n" +
      "  {\n" +
      "    sum += noise( point ) * scale;\n" +
      "    point *= 2.0;\n" +
      "        scale /= 2.0;\n" +
      "  }\n" +
      "    \n" +
      "  return sum;\n" +
      "}\n" +
      "\n" +
      "\n" +
      "void main( )\n" +
      "{\n" +
      "  vec2 point = vTexCoord.xy / uScale;\n" +
      "    //point.x += iGlobalTime * 0.1;\n" +
      "    float noise    = fractal( point * 20.0 );\n" +
      "    \n" +
      "    vec2 uv = vTexCoord.xy; // iResolution.xy;\n" +
      "    //uv.y = 1.0-uv.y;\n" +
      "    gl_FragColor = texture2D(sTexture, uv);\n" +
      "    \n" +
      "    if(noise <  uThresh)\n" +
      "    {\n" +
      "        gl_FragColor = vec4(1.0, 0.5, 0.0, 1.0);\n" +
      "    }\n" +
      "    if(noise <  uThresh - 0.05)\n" +
      "    {\n" +
      "        gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n" +
      "    }\n" +
      "    if(noise <  uThresh - 0.1)\n" +
      "    {\n" +
      "    \n" +
      "        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0); // use discard?\n" +
      "    }\n" +
      "}\n" +
      "\n" +
      "\n" +
      ""
  },
  {
    name: "ripple 2D",
    hints: "",
    animateTo: [ ["uAmplitude", 0.1, "Linear", 0, 3],
                 ["uTime", 6, "Linear", 0, 3] ],
    vertex: "\n" +
      "uniform mediump mat4 uModelView;\n" +
      "uniform mediump mat4 uProjection;\n" +
      "attribute mediump vec3 aPosition;\n" +
      "attribute mediump vec2 aTexCoord;\n" +
      "varying mediump vec2 vTexCoord;\n" +
      "\n" +
      "void main(void)\n" +
      "{\n" +
      "  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n" +
      "  vTexCoord = aTexCoord;\n" +
      "}\n" +
      "\n" +
      "",
    fragment:
    "precision mediump float;\n" +
      "\n" +
      "uniform mediump vec4 uColor;\n" +
      "uniform sampler2D sTexture;\n" +
      "varying mediump vec2 vTexCoord;\n" +
      "\n" +
      "uniform float uAmplitude; // {default:0.02, min:0.0, max:1.0}\n" +
      "uniform float uTime;\n" +
      "uniform mediump vec4 sTextureRect;\n" +
      "void main()\n" +
      "{\n" +
      "  highp vec2 textureSize = sTextureRect.zw - sTextureRect.xy;\n" +
      "  highp vec2 pos = -1.0 + 2.0 * vTexCoord.st/textureSize;\n" +
      "  highp float len = length(pos);\n" +
      "  highp vec2 texCoord = vTexCoord.st/textureSize + pos/len * sin( len * 12.0 - uTime * 4.0 ) * uAmplitude; \n" +
      "  gl_FragColor = texture2D(sTexture, texCoord) * uColor;\n" +
      "}\n" +
      "\n" +
      "\n" +
      ""
  },
  {
    name: "emboss combine",
    hints: "",
    animateTo: [ ["uWeight", [10.0, 10.0, 10.0, 10.0], "Linear", 0, 3] ],
    vertex: "\n" +
      "uniform mediump mat4 uModelView;\n" +
      "uniform mediump mat4 uProjection;\n" +
      "attribute mediump vec3 aPosition;\n" +
      "attribute mediump vec2 aTexCoord;\n" +
      "varying mediump vec2 vTexCoord;\n" +
      "uniform mediump vec3 uSize;\n" +
      "\n" +
      "void main(void)\n" +
      "{\n" +
      "  mediump vec3 vertexPosition = aPosition * uSize;\n" +
      "  gl_Position = uProjection * uModelView * vec4(vertexPosition, 1.0);\n" +
      "  vTexCoord = aTexCoord;\n" +
      "}\n" +
      "\n" +
      "",
    fragment: "precision mediump float;\n" +
      "\n" +
      "uniform mediump vec4 uColor;\n" +
      "uniform sampler2D sTexture;\n" +
      "varying mediump vec2 vTexCoord;\n" +
      "\n" +
      "const int KERNEL_SIZE = 9;\n" +
      "\n" +
      "uniform vec4 uWeight; // {default: [10,10,10,10] }\n" +
      "\n" +
      "// Gaussian kernel\n" +
      "float kernel[KERNEL_SIZE];\n" +
      "\n" +
      "float width  = 512.0;\n" +
      "float height = 512.0;\n" +
      "\n" +
      "float step_w = 1.0/width;\n" +
      "float step_h = 1.0/height;\n" +
      "\n" +
      "vec2 offset[KERNEL_SIZE];\n" +
      "\n" +
      "void main(void)\n" +
      "{\n" +
      "  precision mediump float;\n" +
      "\n" +
      "  vec4 sum = vec4(0.0);\n" +
      "\n" +
      "  offset[0] = vec2(-step_w, -step_h);\n" +
      "  offset[1] = vec2(0.0, -step_h);\n" +
      "  offset[2] = vec2(step_w, -step_h);\n" +
      "\n" +
      "  offset[3] = vec2(-step_w, 0.0);\n" +
      "  offset[4] = vec2(0.0, 0.0);\n" +
      "  offset[5] = vec2(step_w, 0.0);\n" +
      "\n" +
      "  offset[6] = vec2(-step_w, step_h);\n" +
      "  offset[7] = vec2(0.0, step_h);\n" +
      "  offset[8] = vec2(step_w, step_h);\n" +
      "\n" +
      "  // guassian blur\n" +
      "  // kernel[0] = 1.0/16.0;  kernel[1] = 2.0/16.0;  kernel[2] = 1.0/16.0;\n" +
      "  // kernel[3] = 2.0/16.0;  kernel[4] = 4.0/16.0;  kernel[5] = 2.0/16.0;\n" +
      "  // kernel[6] = 1.0/16.0;  kernel[7] = 2.0/16.0;  kernel[8] = 1.0/16.0;\n" +
      "\n" +
      "  // laplace\n" +
      "  // kernel[0] = 0.0;  kernel[1] = 1.0;  kernel[2] = 0.0;\n" +
      "  // kernel[3] = 1.0;  kernel[4] = -4.0; kernel[5] = 1.0;\n" +
      "  // kernel[6] = 0.0;  kernel[7] = 1.0;  kernel[8] = 0.0;\n" +
      "\n" +
      "  // edge\n" +
      "  // kernel[0] = -1.0;  kernel[1] = -1.0; kernel[2] = -1.0;\n" +
      "  // kernel[3] = -1.0;  kernel[4] = +9.0; kernel[5] = -1.0;\n" +
      "  // kernel[6] = -1.0;  kernel[7] = -1.0; kernel[8] = -1.0;\n" +
      "\n" +
      "  // Embossing\n" +
      "  // 2  0  0\n" +
      "  // 0 -1  0\n" +
      "  // 0  0 -1\n" +
      "  kernel[0] = 2.0;  kernel[1] = 0.0;  kernel[2] = 0.0;\n" +
      "  kernel[3] = 0.0;  kernel[4] = -1.0; kernel[5] = 0.0;\n" +
      "  kernel[6] = 0.0;  kernel[7] = 0.0;  kernel[8] =-1.0;\n" +
      "\n" +
      "    vec4 tmp = texture2D(sTexture, vTexCoord.st + offset[0]);\n" +
      "    sum += tmp * kernel[0];\n" +
      "\n" +
      "    tmp = texture2D(sTexture, vTexCoord.st + offset[1]);\n" +
      "    sum += tmp * kernel[1];\n" +
      "\n" +
      "    tmp = texture2D(sTexture, vTexCoord.st + offset[2]);\n" +
      "    sum += tmp * kernel[2];\n" +
      "\n" +
      "    tmp = texture2D(sTexture, vTexCoord.st + offset[3]);\n" +
      "    sum += tmp * kernel[3];\n" +
      "\n" +
      "    tmp = texture2D(sTexture, vTexCoord.st + offset[4]);\n" +
      "    sum += tmp * kernel[4];\n" +
      "\n" +
      "    tmp = texture2D(sTexture, vTexCoord.st + offset[5]);\n" +
      "    sum += tmp * kernel[5];\n" +
      "\n" +
      "    tmp = texture2D(sTexture, vTexCoord.st + offset[6]);\n" +
      "    sum += tmp * kernel[6];\n" +
      "\n" +
      "    tmp = texture2D(sTexture, vTexCoord.st + offset[7]);\n" +
      "    sum += tmp * kernel[7];\n" +
      "\n" +
      "    tmp = texture2D(sTexture, vTexCoord.st + offset[8]);\n" +
      "    sum += tmp * kernel[8];\n" +
      "\n" +
      "    sum = texture2D(sTexture, vTexCoord.xy) + (sum * uWeight);\n" +
      "\n" +
      "  gl_FragColor = sum;\n" +
      "}\n"
  },
  {
    name: "blur",
    hints: "",
    vertex: "\n" +
      "uniform mediump mat4 uModelView;\n" +
      "uniform mediump mat4 uProjection;\n" +
      "uniform mediump vec3 uSize;\n" +
      "attribute mediump vec3 aPosition;\n" +
      "attribute mediump vec2 aTexCoord;\n" +
      "varying mediump vec2 vTexCoord;\n" +
      "\n" +
      "void main(void)\n" +
      "{\n" +
      "  mediump vec3 vertexPosition = aPosition * uSize;\n" +
      "  gl_Position = uProjection * uModelView * vec4(vertexPosition, 1.0);\n" +
      "  vTexCoord = aTexCoord;\n" +
      "}\n" +
      "\n" +
      "",
    fragment:
    "\n" +
      "precision mediump float;\n" +
      "\n" +
      "uniform mediump vec4 uColor;\n" +
      "uniform sampler2D sTexture;\n" +
      "varying mediump vec2 vTexCoord;\n" +
      "\n" +
      "\n" +
      "#define PI2 6.283184\n" +
      "\n" +
      "#define CV 0.1\n" +
      "#define ST 0.05\n" +
      "\n" +
      "uniform float uFactor; // {default: 0.5, min:0.0, max:1.0}\n" +
      "\n" +
      "vec4 colorat(vec2 uv) {\n" +
      "  return texture2D(sTexture, uv);\n" +
      "}\n" +
      "vec4 blur(vec2 uv) { // convolve\n" +
      "  vec4 col = vec4(0.0);\n" +
      "  for(float r0 = 0.0; r0 < 1.0; r0 += ST) {\n" +
      "    float r = r0 * CV;\n" +
      "    for(float a0 = 0.0; a0 < 1.0; a0 += ST) {\n" +
      "      float a = a0 * PI2;\n" +
      "      col += colorat(uv + vec2(cos(a), sin(a)) * r);\n" +
      "    }\n" +
      "  }\n" +
      "  col *= ST * ST;\n" +
      "  return col;\n" +
      "}\n" +
      "\n" +
      "\n" +
      "\n" +
      "void main() {\n" +
      "  vec2 uv = vTexCoord.xy;\n" +
      "  gl_FragColor = blur(uv) * uFactor + ((1.0-uFactor) * texture2D(sTexture, uv));\n" +
      "}\n" +
      "\n"
  },
  {name: "image-click",
   hints: "",
   animateTo: [ ["uRadius", 0.3, "Linear", 0, 0.3] ],
   vertex: "\n" +
   "uniform mediump mat4 uModelView;\n" +
   "uniform mediump mat4 uProjection;\n" +
   "attribute mediump vec3 aPosition;\n" +
   "attribute mediump vec2 aTexCoord;\n" +
   "varying mediump vec2 vTexCoord;\n" +
   "uniform mediump vec3 uSize;\n" +
   "\n" +
   "void main(void)\n" +
   "{\n" +
   "  mediump vec3 vertexPosition = aPosition * uSize;\n" +
   "  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n" +
   "  vTexCoord = aTexCoord;\n" +
   "}\n" +
   "\n" +
   "",
   fragment:
   "precision lowp float;\n" +
   "\n" +
   "uniform mediump vec4 uColor;\n" +
   "varying mediump vec2 vTexCoord;\n" +
   "uniform sampler2D sTexture;\n" +
   "\n" +
   "uniform lowp float uRadius; // {default 0.2, min:0, max:0.3, description:\"touch radius and brightness mix\"\n" +
   "vec2 offset = vec2(0.3,-0.2); // offset from center; for touch point (todo - make uniform)\n" +
   "\n" +
   "void main()\n" +
   "{\n" +
   "  precision lowp float;\n" +
   "  lowp vec2 pos= vec2(vTexCoord.x-0.5-offset.x,vTexCoord.y-0.5-offset.y);\n" +
   "  lowp float radius = dot(pos, pos ) ;\n" +
   "\n" +
   " //  use sRGB correction to brighten image\n" +
   "    vec4 col = texture2D( sTexture, vTexCoord ) * uColor;\n" +
   "    vec3 mask = vec3(greaterThan(col.rgb, vec3(0.0031308)));\n" +
   "    vec3 o = mix(col.rgb * 12.92, \n" +
   "             pow(col.rgb, vec3(1.0/2.4)) * 1.055 - 0.055, \n" +
   "             mask);\n" +
   "    \n" +
   "    vec3 diff = o - col.rgb;\n" +
   "    diff *= uRadius * 3.0;\n" +
   "    o = col.rgb + diff;\n" +
   "    \n" +
   "    gl_FragColor = vec4(o.r, o.g, o.b, col.a);\n" +
   "\n" +
   "  if( radius <= (uRadius*uRadius) )\n" +
   "  {\n" +
   "    gl_FragColor += vec4(0.1);\n" +
   "  }\n" +
   "  \n" +
   "}\n" +
   "\n" +
   ""
  },
  {name: "iris effect",
   hints: "",
   animateTo: [ ["uRadius", 1.0, "Linear", 0, 0.3],
                ["uCenter", [0.2, 0.2], "Linear", 0, 0.0],
                ["uBlendFactor", 1.0, "Linear", 0, 0.0]
              ],
   vertex:
   "\n" +
   "uniform mediump mat4 uModelView;\n" +
   "uniform mediump mat4 uProjection;\n" +
   "attribute mediump vec3 aPosition;\n" +
   "attribute mediump vec2 aTexCoord;\n" +
   "varying mediump vec2 vTexCoord;\n" +
   "uniform mediump vec3 uSize;\n" +
   "\n" +
   "uniform mediump vec2 uCenter; // { default: [0.7, 0.7] } \n" +
   "varying mediump vec2 vRelativePosition;\n" +
   "\n" +
   "void main()\n" +
   "{\n" +
   "    mediump vec3 vertexPosition = aPosition * uSize;\n" +
   "    mediump vec4 world = uModelView * vec4(vertexPosition,1.0);\n" +
   "    gl_Position = uProjection * world;\n" +
   "\n" +
   "    vTexCoord = aTexCoord;\n" +
   "    vRelativePosition = aTexCoord - uCenter;\n" +
   "}\n" +
   "\n" +
   "\n" +
   "",
   fragment:
   "\n" +
   "uniform mediump vec4 uColor;\n" +
   "uniform sampler2D sTexture;\n" +
   "varying mediump vec2 vTexCoord;\n" +
   "\n" +
   "uniform mediump float uRadius; // {default:0.5} \n" +
   "uniform mediump float uBlendFactor; // {default:2} \n" +
   "varying mediump vec2 vRelativePosition;\n" +
   "void main()\n" +
   "{\n" +
   "   mediump float delta = (length(vRelativePosition) - uRadius);\n" +
   "   delta = clamp(0.0 - delta * uBlendFactor, 0.0, 1.0);\n" +
   "   gl_FragColor = texture2D(sTexture, vTexCoord) * uColor;\n" +
   "   gl_FragColor.a *= delta;\n" +
   "}\n" +
   "\n" +
   ""
  },
  {name: "mirror effect",
   hints: "",
   animateTo: [ ["uDepth", 0.5, "Linear", 0, 0.0],
                ["uAlpha", 1.0, "Linear", 0, 0.0]
              ],
   vertex:
   "\n" +
   "uniform mediump mat4 uModelView;\n" +
   "uniform mediump mat4 uProjection;\n" +
   "attribute mediump vec3 aPosition;\n" +
   "attribute mediump vec2 aTexCoord;\n" +
   "varying mediump vec2 vTexCoord;\n" +
   "uniform mediump vec3 uSize;\n" +
   "\n" +
   "void main()\n" +
   "{\n" +
   "  mediump vec3 pos = aPosition * uSize;\n" +
   "  pos.y = pos.y * 3.0;\n" +
   "  mediump vec4 world = uModelView * vec4(pos,1.0);\n" +
   "  gl_Position = uProjection * world;\n" +
   "  vTexCoord = aTexCoord;\n" +
   "}\n" +
   "\n" +
   "",
   fragment:
   "\n" +
   "uniform mediump vec4 uColor;\n" +
   "uniform sampler2D sTexture;\n" +
   "varying mediump vec2 vTexCoord;\n" +
   "\n" +
   "uniform  mediump float  uDepth; // {default: 0.3 }\n" +
   "uniform  mediump float  uAlpha; // {default: 10.0 }\n" +
   "void main()\n" +
   "{\n" +
   " if(vTexCoord.y < 1.0 / 3.0)\n" +
   " {\n" +
   "   gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);\n" +
   " }\n" +
   " else if(vTexCoord.y < 2.0 / 3.0)\n" +
   " {\n" +
   "   gl_FragColor = texture2D(sTexture, vec2(vTexCoord.x, vTexCoord.y * 3.0 - 1.0)) * uColor;\n" +
   "   gl_FragColor.a *= uAlpha;\n" +
   " }\n" +
   " else\n" +
   " {\n" +
   "   highp float darkness = 3.0 - vTexCoord.y * 3.0;\n" +
   "   darkness = (1.0 - 1.0 / uDepth + darkness * 1.0/ uDepth) * 0.65;\n" +
   "   highp vec4 color = texture2D(sTexture, vec2(vTexCoord.x, -vTexCoord.y *3.0 + 3.0)) * uColor;\n" +
   "   color.a *= uAlpha;\n" +
   "   gl_FragColor = color * vec4(darkness, darkness, darkness, darkness);\n" +
   " }\n" +
   "}\n" +
   ""
  },
  {name: "square dissolve",
   animateTo: [ ["uRows", 0.4, "Linear", 0, 0.0],
                ["uColumns", 0.4, "Linear", 0, 0.0],
                ["texSize", [100, 100], "Linear", 0, 0.0],
                ["uStep", 1, "Linear", 0, 3.0]
              ],
   hints: " grid blending",
   vertex: "\n" +
   "uniform mediump mat4 uModelView;\n" +
   "uniform mediump mat4 uProjection;\n" +
   "attribute mediump vec3 aPosition;\n" +
   "attribute mediump vec2 aTexCoord;\n" +
   "varying mediump vec2 vTexCoord;\n" +
   "\n" +
   "void main(void)\n" +
   "{\n" +
   "  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n" +
   "  vTexCoord = aTexCoord;\n" +
   "}\n" +
   "\n" +
   "",
   fragment:
   "\n" +
   "uniform mediump vec4 uColor;\n" +
   "uniform sampler2D sTexture;\n" +
   "varying mediump vec2 vTexCoord;\n" +
   "\n" +
   "uniform  mediump vec2   texSize;\n" +
   "uniform  mediump float  uStep;\n" +
   "uniform  mediump float  uRows;\n" +
   "uniform  mediump float  uColumns;\n" +
   "uniform sampler2D sTexture;\n" +
   "void main()\n" +
   "{\n" +
   "  mediump vec2 mosaicSize = vec2(1.0 / uRows, 1.0 / uColumns);\n" +
   "  mediump vec2 intXY = vec2(vTexCoord.x * texSize.x, vTexCoord.y * texSize.y);\n" +
   "  mediump vec2 XYMosaic = vec2(floor(intXY.x / mosaicSize.x) * mosaicSize.x, floor(intXY.y / mosaicSize.y) * mosaicSize.y);\n" +
   "  mediump vec2 UVMosaic = vec2(XYMosaic.x /texSize.x, XYMosaic.y / texSize.y);\n" +
   "  mediump vec4 noiseVec = texture2D(sEffect, UVMosaic);\n" +
   "  mediump float intensity = (noiseVec[0] + noiseVec[1] + noiseVec[2] + noiseVec[3]) / 4.0;\n" +
   "  if(intensity < uStep)\n" +
   "    gl_FragColor = vec4(0.1, 0.1, 0.1, 1.0);\n" +
   "  else\n" +
   "    gl_FragColor = texture2D(sTexture, vTexCoord);\n" +
   "  gl_FragColor *= uColor;\n" +
   "}\n" +
   "\n" +
   ""
  },
  {name: "swirl",
   hints: " grid blending",
   animateTo: [ ["uCenter", [0.5, 0.5], "Linear", 0, 0.0],
                ["uTextureSize", [100, 100], "Linear", 0, 0.0],
                ["uRadius", 1.0, "Linear", 0, 3.0],
                ["uAngle", 3.0, "Linear", 0, 3.0]
              ],
   vertex: "\n" +
   "uniform mediump mat4 uModelView;\n" +
   "uniform mediump mat4 uProjection;\n" +
   "attribute mediump vec3 aPosition;\n" +
   "attribute mediump vec2 aTexCoord;\n" +
   "varying mediump vec2 vTexCoord;\n" +
   "uniform mediump vec3 uSize;\n" +
   "\n" +
   "void main(void)\n" +
   "{\n" +
   "  mediump vec3 vertexPosition = aPosition * uSize;\n" +
   "  gl_Position = uProjection * uModelView * vec4(vertexPosition, 1.0);\n" +
   "  vTexCoord = aTexCoord;\n" +
   "}\n" +
   "\n" +
   "",
   fragment: "\n" +
   "uniform mediump vec4 uColor;\n" +
   "uniform sampler2D sTexture;\n" +
   "uniform mediump vec4 sTextureRect;\n" +
   "\n" +
   "uniform mediump vec2  uTextureSize;\n" +
   "uniform highp float uRadius;\n" +
   "uniform highp float uAngle;\n" +
   "uniform mediump vec2  uCenter;\n" +
   "varying mediump vec2 vTexCoord;\n" +
   "void main()\n" +
   "{\n" +
   "  highp vec2 textureCenter = (sTextureRect.xy + sTextureRect.zw) * 0.5;\n" +
   "  textureCenter = vTexCoord.st - textureCenter;\n" +
   "  highp float distance = length(textureCenter);\n" +
   "  if (distance >= uRadius)\n" +
   "    discard;\n" +
   "  highp float percent = (uRadius - distance) / uRadius;\n" +
   "  highp float theta = percent * percent * uAngle * 4.0;\n" +
   "  highp float sinTheta = sin(theta);\n" +
   "  highp float cosTheta = cos(theta);\n" +
   "// if warp, loose the sign from sin\n" +
   "  bool warp = true;\n" +
   "  if( warp )\n" +
   "  {\n" +
   "    textureCenter = vec2( dot( textureCenter, vec2(cosTheta, sinTheta) ),\n" +
   "                          dot( textureCenter, vec2(sinTheta, cosTheta) ) );\n" +
   "  }\n" +
   "  else\n" +
   "  {\n" +
   "    textureCenter = vec2( dot( textureCenter, vec2(cosTheta, -sinTheta) ),\n" +
   "                        dot( textureCenter, vec2(sinTheta, cosTheta) ) );\n" +
   "  }\n" +
   "  textureCenter += uCenter;\n" +
   "  gl_FragColor = texture2D( sTexture, textureCenter ) * uColor;\n" +
   "}\n" +
   "\n" +
   "\n" +
   ""
  },
  {name: "drop shadow",
   vertex:
   "\n" +
   "uniform mediump mat4 uModelView;\n" +
   "uniform mediump mat4 uProjection;\n" +
   "attribute mediump vec3 aPosition;\n" +
   "attribute mediump vec2 aTexCoord;\n" +
   "varying mediump vec2 vTexCoord;\n" +
   "uniform mediump vec3 uSize;\n" +
   "\n" +
   "void main()\n" +
   "{\n" +
   "  mediump vec3 pos = aPosition * uSize;\n" +
   "  pos.y = pos.y * 1.1;  \n" +
   "  pos.x = pos.x * 1.1;\n" +
   "  \n" +
   "  mediump vec4 world = uModelView * vec4(pos,1.0);\n" +
   "  gl_Position = uProjection * world;\n" +
   "  vTexCoord = aTexCoord;\n" +
   "}\n" +
   "\n" +
   "\n" +
   "",
   fragment:
   "\n" +
   "uniform mediump vec4 uColor;\n" +
   "uniform sampler2D sTexture;\n" +
   "varying mediump vec2 vTexCoord;\n" +
   "\n" +
   "void main()\n" +
   "{\n" +
   " if(vTexCoord.y <   0.05)\n" +
   " {\n" +
   "   discard;\n" +
   "   gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);\n" +
   " }\n" +
   " else if(vTexCoord.x <   0.05)\n" +
   " {\n" +
   "   discard;\n" +
   "    gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);\n" +
   " }\n" +
   " else if(vTexCoord.y <   0.95 && vTexCoord.x <   0.95)\n" +
   " {\n" +
   "   gl_FragColor = texture2D(sTexture, vec2(vTexCoord.x/ (1.0/1.1) - 0.05, vTexCoord.y / (1.0/1.0) - 0.05 )) * uColor;\n" +
   " }\n" +
   " else\n" +
   " {\n" +
   "   if(vTexCoord.y <   0.1 || vTexCoord.x <   0.1)\n" +
   "   {\n" +
   "       discard;\n" +
   "   }\n" +
   "   else\n" +
   "   {\n" +
   "     gl_FragColor = vec4(0.2, 0.2, 0.2, 1.0);\n" +
   "   }\n" +
   " }\n" +
   "}\n" +
   "\n" +
   "\n" +
   "",
   hints: ""
  },
  {
    name: "noise",
    hints: "",
    vertex: "\n" +
      "uniform mediump mat4 uModelView;\n" +
      "uniform mediump mat4 uProjection;\n" +
      "attribute mediump vec3 aPosition;\n" +
      "attribute mediump vec2 aTexCoord;\n" +
      "varying mediump vec2  vTexCoord;\n" +
      "uniform mediump vec3 uSize;    \n" +
      "\n" +
      "void main(void)\n" +
      "{\n" +
      "  mediump vec3 vertexPosition = aPosition * uSize;\n" +
      "\n" +
      "  gl_Position = uProjection * uModelView * vec4(vertexPosition, 1.0);\n" +
      "  vTexCoord = aTexCoord;\n" +
      "}\n" +
      "\n" +
      "",
    fragment:
    "\n" +
      "precision mediump float;\n" +
      "\n" +
      "uniform mediump vec4 uColor;\n" +
      "varying mediump vec2  vTexCoord;\n" +
      "\n" +
      "// noise\n" +
      "float noise(vec2 pos)\n" +
      "{\n" +
      " return fract( sin( dot(pos*0.001 ,vec2(24.12357, 36.789) ) ) * 12345.123);\n" +
      "}\n" +
      "\n" +
      "\n" +
      "// blur noise\n" +
      "float smooth_noise(vec2 pos)\n" +
      "{\n" +
      " return   ( noise(pos + vec2(1,1)) + noise(pos + vec2(1,1)) + noise(pos + vec2(1,1)) + noise(pos + vec2(1,1)) ) / 16.0\n" +
      "     + ( noise(pos + vec2(1,0)) + noise(pos + vec2(-1,0)) + noise(pos + vec2(0,1)) + noise(pos + vec2(0,-1)) ) / 8.0\n" +
      "       + noise(pos) / 4.0;\n" +
      "}\n" +
      "\n" +
      "\n" +
      "// linear interpolation\n" +
      "float interpolate_noise(vec2 pos)\n" +
      "{\n" +
      "float a, b, c, d;\n" +
      " \n" +
      " a = smooth_noise(floor(pos));\n" +
      " b = smooth_noise(vec2(floor(pos.x+1.0), floor(pos.y)));\n" +
      " c = smooth_noise(vec2(floor(pos.x), floor(pos.y+1.0)));\n" +
      " d = smooth_noise(vec2(floor(pos.x+1.0), floor(pos.y+1.0)));\n" +
      "\n" +
      " a = mix(a, b, fract(pos.x));\n" +
      " b = mix(c, d, fract(pos.x));\n" +
      " a = mix(a, b, fract(pos.y));\n" +
      " \n" +
      " return a;\n" +
      "}\n" +
      "\n" +
      "\n" +
      "float perlin_noise(vec2 pos)\n" +
      "{\n" +
      " float n;\n" +
      " \n" +
      " n = interpolate_noise(pos*0.0625)*0.5;\n" +
      " n += interpolate_noise(pos*0.125)*0.25;\n" +
      " n += interpolate_noise(pos*0.025)*0.225;\n" +
      " n += interpolate_noise(pos*0.05)*0.0625;\n" +
      " n += interpolate_noise(pos)*0.03125;\n" +
      " return n;\n" +
      "}\n" +
      "\n" +
      "\n" +
      "\n" +
      "void main()\n" +
      "{\n" +
      " vec2 pos = vTexCoord.xy;\n" +
      " float c, n;\n" +
      "\n" +
      "\n" +
      " n = perlin_noise(pos);\n" +
      "\n" +
      "\n" +
      " vec2 p = pos; // / iResolution.xy;\n" +
      "\n" +
      " if(p.y <  0.333) // last row\n" +
      "{\n" +
      "  \n" +
      "  if(p.x <  0.333)\n" +
      "    c = abs(cos(n*10.0));\n" +
      "  else if(p.x <  0.666)\n" +
      "    c = cos(pos.x*0.02 + n*10.0);//*0.5+0.5;\n" +
      "  else\n" +
      "  {\n" +
      "    pos *= 0.05;\n" +
      "    c = abs(sin(pos.x+n*5.0)*cos(pos.y+n*5.0));\n" +
      "  }\n" +
      " }\n" +
      " else if(p.y <  0.666) // middle row\n" +
      " {\n" +
      "   \n" +
      "   if(p.x <  0.333)\n" +
      "   {\n" +
      "     pos *= 0.05;\n" +
      "     pos += vec2(10.0, 10.0);\n" +
      "     c = sqrt(pos.x * pos.x + pos.y * pos.y);\n" +
      "    c = fract(c+n);\n" +
      " }\n" +
      " else if(p.x <  0.666)\n" +
      " {\n" +
      "  c = max(1.0 - mod(pos.x*0.5, 80.3+n*4.0)*0.5, 1.0 -  mod(pos.y*0.5, 80.3+n*4.0)*0.5);\n" +
      "  c = max(c, 0.5*max(1.0 - mod(pos.x*0.5+40.0, 80.3+n*4.0)*0.5, 1.0 -  mod(pos.y*0.5+40.0, 80.3+n*4.0)*0.5));\n" +
      " }\n" +
      " else\n" +
      "  c = abs(cos(pos.x*0.1 + n*20.0));// mod(pos.x*0.1, cos(pos.x));\n" +
      " }\n" +
      " else // first row\n" +
      " {\n" +
      "   if(p.x <  0.333)\n" +
      "     c = noise(pos);\n" +
      "   else if(p.x <  0.666)\n" +
      "     c = n;\n" +
      "   else\n" +
      "     c =max(fract(n*20.0), max(fract(n*5.0), fract(n*10.0)));\n" +
      " }\n" +
      "\n" +
      " gl_FragColor = vec4(c, c, c, 1.0);\n" +
      "}\n" +
      "\n" +
      ""
  }
];

//
// Demo Javascript snippets
//
//
var javascriptSources =
[
  {
    name: "XX. Regression tests",
    source: "\n" +
      "clear();\n" +
      "var test = new Test();\n" +
      "test.regression(); // results in developer console (F12)\n" +
      "\n"
  },
  {
    name: "10. ImageView",
    source:"\n" +
      "var a = new dali.ImageView();\n" +
      "\n" +
      "var img = imageFromUiBuffer(\"field\");\n" +
      "\n" +
      "a.setImage(img);\n" +
      "\n" +
      "a.size = [100,100,1];\n" +
      "\n" +
      "dali.stage.add(a)\n" +
      ""
  },
  {
    name: "01. Colored Quad",
    source: "var halfQuadSize = 0.5;\n" +
      "\n" +
      "// using helper function to create property buffer\n" +
      "var verts = dali.createPropertyBuffer( {format: [ [\"aPosition\", dali.PropertyType.VECTOR3],\n" +
      "                                                [\"aCol\", dali.PropertyType.VECTOR4] ],\n" +
      "                                        data: { \"aPosition\": [ [-halfQuadSize, -halfQuadSize, 0.0],\n" +
      "                                                               [+halfQuadSize, -halfQuadSize, 0.0],\n" +
      "                                                               [-halfQuadSize, +halfQuadSize, 0.0],\n" +
      "                                                               [+halfQuadSize, +halfQuadSize, 0.0]\n" +
      "                                                               ],\n" +
      "                                           \"aCol\": [ [0, 0, 0, 1],\n" +
      "                                                       [1, 0, 1, 1],\n" +
      "                                                       [0, 1, 0, 1],\n" +
      "                                                       [1, 1, 1, 1]\n" +
      "                                                     ]\n" +
      "                                             }\n" +
      "                                      }\n" +
      "                                    );\n" +
      "\n" +
      "var indices = dali.createPropertyBuffer( { format: [ [\"indices\", dali.PropertyType.INTEGER]],\n" +
      "                                             data: { \"indices\": [0, 3, 1, 0, 2, 3] } } ) ;\n" +
      "\n" +
      "var geometry = new dali.Geometry();\n" +
      "\n" +
      "geometry.addVertexBuffer(verts);\n" +
      "geometry.setIndexBuffer(indices);\n" +
      "\n" +
      "var vertex = \"\" +\n" +
      "      \"attribute mediump vec3 aPosition;\" +\n" +
      "      \"attribute mediump vec4 aCol;\" +\n" +
      "      \"uniform mediump mat4 uMvpMatrix;\" +\n" +
      "      \"uniform mediump vec3 uSize;\" +\n" +
      "      \"uniform lowp vec4 uColor;\" +\n" +
      "      \"varying lowp vec4 vColor;\" +\n" +
      "      \"\" +\n" +
      "      \"void main()\" +\n" +
      "      \"{\" +\n" +
      "      \"  vColor = aCol * uColor;\" +\n" +
      "      \"  mediump vec4 vertexPosition = vec4(aPosition,1.0);\" +\n" +
      "      \"  vertexPosition.xyz *= uSize;\" +\n" +
      "      \"  gl_Position = uMvpMatrix * vertexPosition;\" +\n" +
      "      \"}\";\n" +
      "\n" +
      "var fragment = \"\" +\n" +
      "      \"varying lowp vec4 vColor;\" +\n" +
      "      \"uniform lowp vec4 uColor;\" +\n" +
      "      \"\" +\n" +
      "      \"void main()\" +\n" +
      "      \"{\" +\n" +
      "      \"  gl_FragColor = vColor * uColor;\" +\n" +
      "      \"}\";\n" +
      "\n" +
      "var shader = new dali.Shader(vertex, fragment, dali.ShaderHints.HINT_NONE);\n" +
      "\n" +
      "var material = new dali.Material(shader);\n" +
      "\n" +
      "var renderer = new dali.Renderer(geometry, material);\n" +
      "\n" +
      "var actor = new dali.Actor();\n" +
      "\n" +
      "actor.addRenderer(renderer);\n" +
      "\n" +
      "dali.stage.add(actor);\n" +
      "\n" +
      "actor.parentOrigin = [0.5, 0.5, 0.0];\n" +
      "actor.size = [100,100,1];\n" +
      "\n" +
      ""
  },
  {
    name: "02. Textured Quad",
    source: "var halfQuadSize = 0.5;\n" +
      "\n" +
      "// using helper function to create property buffer\n" +
      "var verts = dali.createPropertyBuffer( {format: [ [\"aPosition\", dali.PropertyType.VECTOR3],\n" +
      "                                                  [\"aTexCoord\", dali.PropertyType.VECTOR2] ],\n" +
      "                                          data: { \"aPosition\": [ [-halfQuadSize, -halfQuadSize, 0.0],\n" +
      "                                                                 [+halfQuadSize, -halfQuadSize, 0.0],\n" +
      "                                                                 [-halfQuadSize, +halfQuadSize, 0.0],\n" +
      "                                                                 [+halfQuadSize, +halfQuadSize, 0.0]\n" +
      "                                                               ],\n" +
      "                                                  \"aTexCoord\": [ [0, 0],\n" +
      "                                                                 [1, 0],\n" +
      "                                                                 [0, 1],\n" +
      "                                                                 [1, 1]\n" +
      "                                                               ]\n" +
      "                                                }\n" +
      "                                     });\n" +
      "\n" +
      "var indices = dali.createPropertyBuffer( { format: [ [\"indices\", dali.PropertyType.INTEGER]],\n" +
      "                                           data: { \"indices\": [0, 3, 1, 0, 2, 3] } } ) ;\n" +
      "\n" +
      "var geometry = new dali.Geometry();\n" +
      "\n" +
      "geometry.addVertexBuffer(verts);\n" +
      "geometry.setIndexBuffer(indices);\n" +
      "\n" +
      "var shader = shaderFromUiBuffer(\"pass through texture\");\n" +
      "\n" +
      "var material = new dali.Material(shader);\n" +
      "\n" +
      "var image = imageFromUiBuffer(\"ducks\");\n" +
      "var sampler = new dali.Sampler();\n" +
      "material.addTexture(image, \"sTexture\", sampler);\n" +
      "\n" +
      "var renderer = new dali.Renderer(geometry, material);\n" +
      "\n" +
      "var actor = new dali.Actor();\n" +
      "\n" +
      "actor.addRenderer(renderer);\n" +
      "\n" +
      "dali.stage.add(actor);\n" +
      "\n" +
      "actor.parentOrigin = [0.5, 0.5, 0.0];\n" +
      "actor.size = [100,100,1];\n" +
      "\n" +
      "\n" +
      ""
  },
  {
    name: "03. Shaded Quads",
    source: "var halfQuadSize = 0.5;\n" +
      "\n" +
      "// using helper function to create property buffer\n" +
      "var verts = dali.createPropertyBuffer( {format: [ [\"aPosition\", dali.PropertyType.VECTOR3],\n" +
      "                                                  [\"aTexCoord\", dali.PropertyType.VECTOR2] ],\n" +
      "                                          data: { \"aPosition\": [ [-halfQuadSize, -halfQuadSize, 0.0],\n" +
      "                                                                 [+halfQuadSize, -halfQuadSize, 0.0],\n" +
      "                                                                 [-halfQuadSize, +halfQuadSize, 0.0],\n" +
      "                                                                 [+halfQuadSize, +halfQuadSize, 0.0]\n" +
      "                                                               ],\n" +
      "                                                  \"aTexCoord\": [ [0, 0],\n" +
      "                                                                 [1, 0],\n" +
      "                                                                 [0, 1],\n" +
      "                                                                 [1, 1]\n" +
      "                                                               ]\n" +
      "                                                }\n" +
      "                                     });\n" +
      "\n" +
      "\n" +
      "var indices = dali.createPropertyBuffer( { format: [ [\"indices\", dali.PropertyType.INTEGER]],\n" +
      "                                      data: { \"indices\": [0, 3, 1, 0, 2, 3] } } ) ;\n" +
      "\n" +
      "var geometry = new dali.Geometry();\n" +
      "\n" +
      "geometry.addVertexBuffer(verts);\n" +
      "geometry.setIndexBuffer(indices);\n" +
      "\n" +
      "\n" +
      "// some shader buffers for textured quad\n" +
      "var names = [\"adjust brightness\", \"blur\", \"burn\", \"drop shadow\", \"emboss combine\",\n" +
      "             \"image-click\", \"melt\", \"ripple 2D\", \"mirror effect\", \"iris effect\",\n" +
      "             \"mosaic\", \"round window\", \"swirl\", \"noise\"];\n" +
      "\n" +
      "var n = Math.floor(Math.sqrt(names.length));\n" +
      "var c = 0;\n" +
      "var x = -(0.5*n*100);\n" +
      "var y = -(0.5*n*100);\n" +
      "\n" +
      "for(var i = 0; i < names.length; i++) {\n" +
      "  var shader = shaderFromUiBuffer(names[i]);\n" +
      "\n" +
      "  var material = new dali.Material(shader);\n" +
      "\n" +
      "  var image = imageFromUiBuffer(\"ducks\");\n" +
      "  var sampler = new dali.Sampler();\n" +
      "  material.addTexture(image, \"sTexture\", sampler);\n" +
      "\n" +
      "  var renderer = new dali.Renderer(geometry, material);\n" +
      "\n" +
      "  var actor = new dali.Actor();\n" +
      "\n" +
      "  actor.name =  \"actor for shader:\" + names[i];\n" +
      "\n" +
      "  actor.addRenderer(renderer);\n" +
      "\n" +
      "  dali.stage.add(actor);\n" +
      "\n" +
      "  actor.parentOrigin = [0.5, 0.5, 0.0];\n" +
      "  actor.size = [100,100,1];\n" +
      "\n" +
      "  actor.position = [x,\n" +
      "                    y + (c*110), \n" +
      "                    0];\n" +
      "\n" +
      "  if(c>n) {\n" +
      "     c = 0;\n" +
      "     x += 110;\n" +
      "  } else {\n" +
      "      c += 1;\n" +
      "  }\n" +
      "\n" +
      "}\n" +
      "\n" +
      ""
  },
  {
    name: "04. Animation",
    source: "var halfQuadSize = 0.5;\n" +
      "\n" +
      "// use helper function to create property buffer\n" +
      "var verts = dali.createPropertyBuffer( {format: [ [\"aPosition\", dali.PropertyType.VECTOR3],\n" +
      "                                                  [\"aTexCoord\", dali.PropertyType.VECTOR2] ],\n" +
      "                                          data: { \"aPosition\": [ [-halfQuadSize, -halfQuadSize, 0.0],\n" +
      "                                                                 [+halfQuadSize, -halfQuadSize, 0.0],\n" +
      "                                                                 [-halfQuadSize, +halfQuadSize, 0.0],\n" +
      "                                                                 [+halfQuadSize, +halfQuadSize, 0.0]\n" +
      "                                                               ],\n" +
      "                                                  \"aTexCoord\": [ [0, 0],\n" +
      "                                                                 [1, 0],\n" +
      "                                                                 [0, 1],\n" +
      "                                                                 [1, 1]\n" +
      "                                                               ]\n" +
      "                                                }\n" +
      "                                     });\n" +
      "\n" +
      "var indices = dali.createPropertyBuffer( { format: [ [\"indices\", dali.PropertyType.INTEGER]],\n" +
      "                                           data: { \"indices\": [0, 3, 1, 0, 2, 3] } } ) ;\n" +
      "\n" +
      "var geometry = new dali.Geometry();\n" +
      "\n" +
      "geometry.addVertexBuffer(verts);\n" +
      "geometry.setIndexBuffer(indices);\n" +
      "\n" +
      "var vertex = \"\" + \n" +
      "      \"// atributes\\n\" + \n" +
      "      \"attribute mediump vec3 aPosition;\\n\" + \n" +
      "      \"attribute mediump vec2 aTexCoord;\\n\" + \n" +
      "      \"// inbuilt\\n\" + \n" +
      "      \"uniform mediump mat4 uMvpMatrix;\\n\" + \n" +
      "      \"uniform mediump vec3 uSize;\\n\" + \n" +
      "      \"uniform lowp vec4 uColor;\\n\" + \n" +
      "      \"// varying\\n\" + \n" +
      "      \"varying mediump vec2 vTexCoord;\\n\" + \n" +
      "      \"\\n\" + \n" +
      "      \"void main()\\n\" + \n" +
      "      \"{\\n\" + \n" +
      "      \"  mediump vec4 vertexPosition = vec4(aPosition, 1.0);\\n\" + \n" +
      "      \"  vertexPosition.xyz *= uSize;\\n\" + \n" +
      "      \"  gl_Position = uMvpMatrix * vertexPosition;\\n\" + \n" +
      "      \"  vTexCoord = aTexCoord;\\n\" + \n" +
      "      \"}\";\n" +
      "\n" +
      "var fragment = \"\" + \n" +
      "      \"uniform lowp vec4 uColor;\\n\" + \n" +
      "      \"uniform sampler2D sTexture;\\n\" + \n" +
      "      \"varying mediump vec2 vTexCoord;\\n\" + \n" +
      "      \"\\n\" + \n" +
      "      \"void main()\\n\" + \n" +
      "      \"{\\n\" + \n" +
      "      \"  gl_FragColor = texture2D(sTexture, vTexCoord) * uColor;\\n\" + \n" +
      "      \"}\";\n" +
      "\n" +
      "var shader = new dali.Shader(vertex, fragment, dali.ShaderHints.HINT_NONE);\n" +
      "\n" +
      "var material = new dali.Material(shader);\n" +
      "\n" +
      "var image = imageFromUiBuffer(\"ducks\");\n" +
      "var sampler = new dali.Sampler();\n" +
      "material.addTexture(image, \"sTexture\",sampler);\n" +
      "\n" +
      "var renderer = new dali.Renderer(geometry, material);\n" +
      "\n" +
      "var actor = new dali.Actor();\n" +
      "\n" +
      "actor.addRenderer(renderer);\n" +
      "\n" +
      "dali.stage.add(actor);\n" +
      "\n" +
      "actor.parentOrigin = [0.5, 0.5, 0.0];\n" +
      "actor.size = [100,100,1];\n" +
      "\n" +
      "\n" +
      "var animation = new dali.Animation(9);\n" +
      "\n" +
      "animation.animateTo(actor, \"orientation\", [0,90,0], \"linear\", 0, 3);\n" +
      "animation.animateTo(actor, \"orientation\", [0,0,0], \"linear\", 6, 3);\n" +
      "\n" +
      "var p = new dali.Path();\n" +
      "\n" +
      "p.addPoint( [0,0,0] );\n" +
      "p.addPoint( [200,200,200] );\n" +
      "p.addPoint( [0,0,-200] );\n" +
      "p.addPoint( [0,-200,-900] );\n" +
      "p.addPoint( [0,0,0] );\n" +
      "\n" +
      "p.generateControlPoints(0.43);\n" +
      "\n" +
      "animation.animatePath( actor, p, [1,0,0], \"linear\", 0, 6);\n" +
      "\n" +
      "//animation.setEndAction(dali.EndAction.Discard);\n" +
      "\n" +
      "animation.play();\n" +
      "\n" +
      "\n" +
      ""
  },
  {
    name: "05. Event driven Animation",
    source: "var halfQuadSize = 0.5;\n" +
      "\n" +
      "// use helper function to create property buffer\n" +
      "var verts = dali.createPropertyBuffer( {format: [ [\"aPosition\", dali.PropertyType.VECTOR3],\n" +
      "                                                  [\"aTexCoord\", dali.PropertyType.VECTOR2] ],\n" +
      "                                          data: { \"aPosition\": [ [-halfQuadSize, -halfQuadSize, 0.0],\n" +
      "                                                                 [+halfQuadSize, -halfQuadSize, 0.0],\n" +
      "                                                                 [-halfQuadSize, +halfQuadSize, 0.0],\n" +
      "                                                                 [+halfQuadSize, +halfQuadSize, 0.0]\n" +
      "                                                               ],\n" +
      "                                                  \"aTexCoord\": [ [0, 0],\n" +
      "                                                                 [1, 0],\n" +
      "                                                                 [0, 1],\n" +
      "                                                                 [1, 1]\n" +
      "                                                               ]\n" +
      "                                                }\n" +
      "                                     });\n" +
      "\n" +
      "var indices = dali.createPropertyBuffer( { format: [ [\"indices\", dali.PropertyType.INTEGER]],\n" +
      "                                           data: { \"indices\": [0, 3, 1, 0, 2, 3] } } ) ;\n" +
      "\n" +
      "var geometry = new dali.Geometry();\n" +
      "\n" +
      "geometry.addVertexBuffer(verts);\n" +
      "geometry.setIndexBuffer(indices);\n" +
      "\n" +
      "var vertex = \"\" +\n" +
      "      \"// atributes\\n\" + \n" +
      "      \"attribute mediump vec3 aPosition;\\n\" + \n" +
      "      \"attribute mediump vec2 aTexCoord;\\n\" + \n" +
      "      \"// inbuilt\\n\" + \n" +
      "      \"uniform mediump mat4 uMvpMatrix;\\n\" + \n" +
      "      \"uniform mediump vec3 uSize;\\n\" + \n" +
      "      \"uniform lowp vec4 uColor;\\n\" + \n" +
      "      \"// varying\\n\" + \n" +
      "      \"varying mediump vec2 vTexCoord;\\n\" + \n" +
      "      \"\\n\" + \n" +
      "      \"void main()\\n\" + \n" +
      "      \"{\\n\" + \n" +
      "      \"  mediump vec4 vertexPosition = vec4(aPosition, 1.0);\\n\" + \n" +
      "      \"  vertexPosition.xyz *= uSize;\\n\" + \n" +
      "      \"  gl_Position = uMvpMatrix * vertexPosition;\\n\" + \n" +
      "      \"  vTexCoord = aTexCoord;\\n\" + \n" +
      "      \"}\";\n" +
      "\n" +
      "var fragment = \"\" +\n" +
      "      \"uniform lowp vec4 uColor;\\n\" + \n" +
      "      \"uniform sampler2D sTexture;\\n\" + \n" +
      "      \"varying mediump vec2 vTexCoord;\\n\" + \n" +
      "      \"\\n\" + \n" +
      "      \"void main()\\n\" + \n" +
      "      \"{\\n\" + \n" +
      "      \"  gl_FragColor = texture2D(sTexture, vTexCoord) * uColor;\\n\" + \n" +
      "      \"}\";\n" +
      "\n" +
      "var shader = new dali.Shader(vertex, fragment, dali.ShaderHints.HINT_NONE);\n" +
      "\n" +
      "var material = new dali.Material(shader);\n" +
      "\n" +
      "var image = imageFromUiBuffer(\"ducks\");\n" +
      "var sampler = new dali.Sampler();\n" +
      "material.addTexture(image, \"sTexture\", sampler);\n" +
      "\n" +
      "var renderer = new dali.Renderer(geometry, material);\n" +
      "\n" +
      "var actor = new dali.Actor();\n" +
      "\n" +
      "actor.addRenderer(renderer);\n" +
      "\n" +
      "dali.stage.add(actor);\n" +
      "\n" +
      "actor.parentOrigin = [0.5, 0.5, 0.0];\n" +
      "actor.size = [100,100,1];\n" +
      "\n" +
      "var actor2 = new dali.Actor();\n" +
      "\n" +
      "// @todo?? why can I no reuse the same renderer?\n" +
      "//actor2.addRenderer(renderer);\n" +
      "var renderer2 = new dali.Renderer(geometry, material);\n" +
      "actor2.addRenderer(renderer2);\n" +
      "\n" +
      "dali.stage.add(actor2);\n" +
      "\n" +
      "actor2.position = [-200,0,0];\n" +
      "actor2.parentOrigin = [0.5, 0.5, 0.0];\n" +
      "actor2.size = [100,100,1];\n" +
      "\n" +
      "var animation = new dali.Animation(9);\n" +
      "\n" +
      "animation.animateTo(actor, \"orientation\", [0,90,0], \"linear\", 0, 3);\n" +
      "animation.animateTo(actor, \"orientation\", [0,0,0], \"linear\", 6, 3);\n" +
      "\n" +
      "var p = new dali.Path();\n" +
      "\n" +
      "p.addPoint( [0,0,0] );\n" +
      "p.addPoint( [200,200,200] );\n" +
      "p.addPoint( [0,0,-200] );\n" +
      "p.addPoint( [0,-200,-900] );\n" +
      "p.addPoint( [0,0,0] );\n" +
      "\n" +
      "p.generateControlPoints(0.43);\n" +
      "\n" +
      "animation.animatePath( actor, p, [1,0,0], \"linear\", 0, 6);\n" +
      "\n" +
      "function onTouched(actor, e) {\n" +
      "  for(var i = 0; i < e.points.length; i++) {\n" +
      "      if(e.points[i].state === \"Down\") {\n" +
      "        console.log(e);\n" +
      "        animation.play();\n" +
      "        return;\n" +
      "      }\n" +
      "  }\n" +
      "}\n" +
      "\n" +
      "actor2.connect(\"touched\", onTouched);\n" +
      "\n" +
      "\n" +
      ""
  },
  {
    name: "06. Offscreen",
    source: "\n" +
      "// todo"
  },
  {
    name: "07. DALi toy pseudo dsl",
    source: "\n" +
      "//\n" +
      "// @todo\n" +
      "//\n" +
      "addTo(\"stage\",\n" +
      "      image({\n" +
      "        name:\"img\",\n" +
      "        image:\"ducks\",\n" +
      "        size:[100,100,1]\n" +
      "      }),\n" +
      "      image({\n" +
      "        name:\"img2\",\n" +
      "        tag:\"listitem\",\n" +
      "        image:\"field\",\n" +
      "        position:[100,0,0],\n" +
      "        size:[100,100,1]\n" +
      "      }),\n" +
      "      image({\n" +
      "        name:\"img3\",\n" +
      "        tag:\"listitem\",\n" +
      "        image:\"funnyface\",\n" +
      "        position:[100,0,0],\n" +
      "        size:[100,100,1]\n" +
      "      }),\n" +
      "      image({\n" +
      "        name:\"img4\",\n" +
      "        tag:\"listitem\",\n" +
      "        image:\"girl1\",\n" +
      "        position:[100,0,0],\n" +
      "        size:[100,100,1]\n" +
      "      })\n" +
      ");\n" +
      "\n" +
      "\n" +
      "when(\"img\", \"touchedDown\",\n" +
      "    set(\"sensitive\", true),\n" +
      "    set(\"size\", to([200,200,200])),\n" +
      "    set(\"img2\", \"size\", to([200,200,200], 0,3, \"ease_in\")),\n" +
      "    set(excludeFrom(tagged(\"scrollitem\"), \"myimage\"), path(0,3, \"ease_in\", \"path0\")),\n" +
      "    endAction(\"Discard\")\n" +
      "    then(set(\"img4\", \"hide\"),\n" +
      "         play(\"anAnim\"))    );\n" +
      "    \n" +
      "\n" +
      "\n"
  }
];

function log(errorLog) {
  console.log(errorLog);
}

function consoleAssert( test, message )
{
  // using this rather than inbuild assert
  // it lets dali carry on for some reason
  if(!test)
  {
    throw message;
  }
}

function consoleLogErrorEvent(event) {
  "use strict";
  console.log("Error");
  console.dir(event);
  if(event.target) {
    if(event.target.result) {
      event.target.result.close();
    }
  }
}

function consoleLogSuccess(message) {
  "use strict";
  return function() {
    console.log("Success:" + message);
  };
}


/**
 * Gets embedded base64 images embedded in the HTML file
 */
function getStockImageData(index) {
  "use strict";
  var name = "testImage" + index;
  // need to draw it off screen first? @todo
  var c = document.createElement("canvas");
  var img = document.getElementById(name);
  c.width = img.naturalWidth;
  c.height = img.naturalHeight;
  var context = c.getContext("2d");
  context.drawImage(img, 0, 0 );
  var imageData = context.getImageData(0, 0, img.naturalWidth, img.naturalHeight); // <-ImageData
  return imageData;
}

function formatValue(value)
{
  if(typeof(value) === "number") {
    if(value === 3.4028234663852886e+38)
    {
      return "MAX_FLOAT";
    } else if(value === Number.MAX_VALUE) {
      return "MAX_DBL";
    } else if(value === 2147483647) {
      return "MAX_INT";
    } else if(value === 9223372036854775807) {
      return "MAX_INT64";
    } else {
      return value.toFixed(3);
    }
  } else {
    return value;
  }
}

/**
 * Add bootstrap column for single value (not array)
 */
function add1ColElement(elem, value)
{
  var e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = formatValue(value);
  elem.appendChild(e);

  e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = "";
  elem.appendChild(e);

  e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = "";
  elem.appendChild(e);

  e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = "";
  elem.appendChild(e);

  return e;
}

/*
 * Add bootstrap column for array of 2 value
 */
function add2ColElement(elem, value)
{
  var e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = formatValue(value[0]);
  elem.appendChild(e);

  e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = formatValue(value[1]);
  elem.appendChild(e);

  e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = "";
  elem.appendChild(e);

  e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = "";
  elem.appendChild(e);

  return e;
}

/*
 * Add bootstrap table cell for array of 3 value
 */
function add3ColElement(elem, value)
{
  var e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = formatValue(value[0]);
  elem.appendChild(e);

  e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = formatValue(value[1]);
  elem.appendChild(e);

  e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = formatValue(value[2]);
  elem.appendChild(e);

  e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = "";
  elem.appendChild(e);
  return e;
}

/*
 * Add bootstrap table cell for array of 4 value
 */
function add4ColElement(elem, value)
{
  var e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = formatValue(value[0]);
  elem.appendChild(e);

  e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = formatValue(value[1]);
  elem.appendChild(e);

  e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = formatValue(value[2]);
  elem.appendChild(e);

  e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = formatValue(value[3]);
  elem.appendChild(e);
  return e;
}

/*
 * Add a bootstrap table cell for property 'name' for an actor
 */
function createElementForActorProperty(actor, name) {
  "use strict";
  var elem = document.createElement("div");

  var value = actor[name];
  var type = typeof value;

  if(name === "maximumSize") {
    value = value;
  }

  if(type === "string" || type === "number" || type === "boolean") {
    elem.className = "row";
    add1ColElement(elem, value);
  } else if(value === undefined) {
    elem.className = "row";
    add1ColElement(elem, "???undefined???");
  } else {
    var r;
    var length = value.length;
    if(length === 2) {
      elem.className = "row";
      add2ColElement(elem, value);
    } else if(length === 3) {
      elem.className = "row";
      add3ColElement(elem, value);
    } else if(length === 4) {
      elem.className = "row";
      add4ColElement(elem, value);
    } else if(length === 9) {
      r = document.createElement("div");
      r.className = "row";
      add3ColElement(r, value.slice(0, 3));
      elem.appendChild(r);

      r = document.createElement("div");
      r.className = "row";
      add3ColElement(r, value.slice(3, 6));
      elem.appendChild(r);

      r = document.createElement("div");
      r.className = "row";
      add3ColElement(r, value.slice(6, 9));
      elem.appendChild(r);
    } else if(length === 16) {
      r = document.createElement("div");
      r.className = "row";
      add4ColElement(r, value.slice(0, 4));
      elem.appendChild(r);

      r = document.createElement("div");
      r.className = "row";
      add4ColElement(r, value.slice(4, 8));
      elem.appendChild(r);

      r = document.createElement("div");
      r.className = "row";
      add4ColElement(r, value.slice(8, 12));
      elem.appendChild(r);

      r = document.createElement("div");
      r.className = "row";
      add4ColElement(r, value.slice(12, 16));
      elem.appendChild(r);
    }
  }

  return elem;
}

/**
 * Adds a bootstrap table to show the actor properties
 */
function onActorSelected(actor) {
  "use strict";
  var i;
  var name;
  var gridBlock;
  var nameBlock;
  var valueBlock;
  var actorId;

  //
  // recreate property tab
  //
  var elem = document.getElementById("selected");

  removeAllChildren(elem);

  //
  // setup property view
  //
  if (actor) {
    actorId = actor.getId();

    var p = document.createElement("div");
    p.innerHTML = "<h3><b>" + actor.name + "</b> [<b>" + actorId + "</b>] (<b>" + actor.position + "</b>)</h3>";
    elem.appendChild(p);

    var parent = actor.getParent();
    if(parent) {
      p = document.createElement("div");
      p.innerHTML =
        "<p>" +
        "(Parent: <b>" + parent.name + "</b> [<b>" + parent.getId() + "</b>] (<b>" + parent.position + "</b>)" +
        " Anchored: <b>" + actor.anchorPoint + "</b>" +
        " ParentOrigin: <b>" + actor.parentOrigin + "</b> )" +
        "</p>";
      elem.appendChild(p);
    }

    if( actorId in actorIdToShaderSet &&
        "shaderEffect" in actorIdToShaderSet[actorId]) { // if has compiled
      var shaderOptions = actorIdToShaderSet[actorId];
      var uniforms = dali.sourceUniformMetaData(shaderOptions.vertex);
      uniforms = uniforms.concat(dali.sourceUniformMetaData(shaderOptions.fragment));

      var shader = getShader(actor);

      if(uniforms.length) {
        gridBlock = document.createElement("div");
        gridBlock.className = "row";

        nameBlock = document.createElement("div");
        nameBlock.className = "col-md-5";
        nameBlock.innerHTML = "<u>Shader Uniforms:</u>";
        gridBlock.appendChild(nameBlock);

        valueBlock = document.createElement("div");
        valueBlock.className = "col-md-7";
        valueBlock.innerHTML = "";
        gridBlock.appendChild(valueBlock);

        elem.appendChild(gridBlock);

        for(i = 0; i < uniforms.length; i++) {
          var type = uniforms[i].type;
          name = uniforms[i].name;
          gridBlock = document.createElement("div");
          gridBlock.className = "row";

          nameBlock = document.createElement("div");
          nameBlock.className = "col-md-5";
          nameBlock.innerHTML = type + " " + name + "<i> (Animatable) </i>";

          gridBlock.appendChild(nameBlock);

          valueBlock = document.createElement("div");
          valueBlock.className = "col-md-7";
          valueBlock.innerHTML = shader[ name ];
          gridBlock.appendChild(valueBlock);

          elem.appendChild(gridBlock);

        }
      }

    }

    // getproperties returns std::vector<string>
    var props = actor.getProperties();

    gridBlock = document.createElement("div");
    gridBlock.className = "row";

    nameBlock = document.createElement("div");
    nameBlock.className = "col-md-5";
    nameBlock.innerHTML = "<u>Properties:</ul>";
    gridBlock.appendChild(nameBlock);

    valueBlock = document.createElement("div");
    valueBlock.className = "col-md-7";
    valueBlock.innerHTML = "";
    gridBlock.appendChild(valueBlock);

    elem.appendChild(gridBlock);


    for (i = 0; i < props.size(); i++) {
      name = props.get(i);

      gridBlock = document.createElement("div");
      gridBlock.className = "row";

      var animatable = actor.isPropertyAnimatable(actor.getPropertyIndex(name));

      nameBlock = document.createElement("div");
      nameBlock.className = "col-md-5";
      if(animatable) {
        nameBlock.innerHTML = "'" + name + "'" + "<i> (animatable)</i>";
      } else {
        nameBlock.innerHTML = "'" + name + "'";
      }
      gridBlock.appendChild(nameBlock);

      valueBlock = document.createElement("div");
      valueBlock.className = "col-md-7";

      valueBlock.appendChild( createElementForActorProperty(actor, name) );

      gridBlock.appendChild(valueBlock);

      elem.appendChild(gridBlock);
    }

    props.delete();

  }

}

/**
 * creates page element and set namesValues array
 */
function createElement(elementName, namesValues) {
  "use strict";
  var e = document.createElement(elementName);
  for(var name in namesValues) {
    e[name] = namesValues[name];
  }
  return e;
}

/**
 * create element with innerHTML
 */
function createElementInnerHTML(elementName, innerHTML) {
  "use strict";
  return createElement(elementName, {"innerHTML": innerHTML});
}

/**
 * remove all the children from the element
 */
function removeAllChildren(elem) {
  "use strict";
  var count = elem.children.length;
  for (var i = 0; i < count; i++) {
    elem.removeChild(elem.children[0]);
  }
}

/**
 *
 */
function onChangeSetDataChanged(e) {
  e.currentTarget["data-changed"] = true;
}

/**
 * creates a document input element [ ]
 */
function inputElem(type, propObject, property, val, arrayIndex) {
  "use strict";
  var e = document.createElement("input");
  e.type = type;
  // for number then step of any means it can be a real number not just integer
  e.step = "any";
  e.value = val;
  e.className = "form-control";
  e["data-value"] = val;
  e["data-arrayIndex"] = arrayIndex;
  e["data-changed"] = false;
  e.addEventListener("changed", onChangeSetDataChanged);
  return e;
}

/**
 * creates an input 2 element [ , ]
 */
function inputElem2(type, propObject, property, val, startIndex) {
  "use strict";
  var d = document.createElement("div");
  var a = inputElem("number", propObject, property, val[startIndex], startIndex);
  a.className = "col-md-3";
  d.appendChild(a);
  a = inputElem("number", propObject, property, val[startIndex + 1], startIndex + 1);
  a.className = "col-md-3";
  d.appendChild(a);
  a.className = "col-md-3";
  d.appendChild(a);
  a = createElement("div");
  a.className = "col-md-3";
  d.appendChild(a);
  return d;
}

/**
 * creates an input 3 element [ , , ]
 */
function inputElem3(type, propObject, property, val, startIndex) {
  "use strict";
  var d = document.createElement("div");
  var a = inputElem("number", propObject, property, val[startIndex], startIndex);
  a.className = "col-md-3";
  d.appendChild(a);
  a = inputElem("number", propObject, property, val[startIndex + 1], startIndex + 1);
  a.className = "col-md-3";
  d.appendChild(a);
  a = inputElem("number", propObject, property, val[startIndex + 2], startIndex + 2);
  a.className = "col-md-3";
  d.appendChild(a);
  a = createElement("div");
  a.className = "col-md-3";
  d.appendChild(a);
  return d;
}

/**
 * creates an input 4 element [ , , , ]
 */
function inputElem4(type, propObject, property, val, startIndex) {
  "use strict";
  var d = document.createElement("div");
  var a = inputElem("number", propObject, property, val[startIndex], startIndex);
  a.className = "col-md-3";
  d.appendChild(a);
  a = inputElem("number", propObject, property, val[startIndex + 1], startIndex + 1);
  a.className = "col-md-3";
  d.appendChild(a);
  a = inputElem("number", propObject, property, val[startIndex + 2], startIndex + 2);
  a.className = "col-md-3";
  d.appendChild(a);
  a = inputElem("number", propObject, property, val[startIndex + 3], startIndex + 3);
  a.className = "col-md-3";
  d.appendChild(a);
  return d;
}

/**
 * creates a document input element for an actor/shader's property
 */
function createInputElement(actorShader, propertyName, value) {
// always4 ja columns
  "use strict";
  var type = typeof value;

  var e;
  if(type === "string") {
    e = inputElem("string", actorShader, propertyName, value);
  } else if(type === "number") {
    e = inputElem("number", actorShader, propertyName, value);
  } else if(type === "boolean") {
    e = inputElem("checkbox", actorShader, propertyName, value);
  } else {
    var length = value.length;
    if(length === 2) {
      e = inputElem2("number", actorShader, propertyName, value, 0);
    } else if(length === 3) {
      e = inputElem3("number", actorShader, propertyName, value, 0);
    } else if(length === 4) {
      e = inputElem4("number", actorShader, propertyName, value, 0);
    } else if(length === 9) {
      e = createElement("div");
      e.appendChild( inputElem3("number", actorShader, propertyName, value, 0) );
      e.appendChild( inputElem3("number", actorShader, propertyName, value, 3) );
      e.appendChild( inputElem3("number", actorShader, propertyName, value, 6) );
    } else if(length === 16) {
      e = createElement("div");
      e.appendChild( inputElem4("number", actorShader, propertyName, value, 0) );
      e.appendChild( inputElem4("number", actorShader, propertyName, value, 4) );
      e.appendChild( inputElem4("number", actorShader, propertyName, value, 8) );
      e.appendChild( inputElem4("number", actorShader, propertyName, value, 12) );
    } else {
      throw "should not happen";
    }
  }

  return e;
}

/**
 * Selects an actor by id
 */
function selectActor(id) { // from html tree
  "use strict";
  if(id === null) {
    eventHandler.selectActor(null);
  } else {
    var root = dali.stage.getRootLayer();
    var actor = root.findChildById(id);
    if (actor) {
      eventHandler.selectActor(actor);
    }
    root.delete(); // wrapper
  }
}

/**
 * Rebuild the document actor property display
 */
function rebuildTree() {
  "use strict";
  // remove childred
  var e = document.getElementById("tree");
  var count = e.children.length;
  for (var i = 0; i < count; i++) {
    e.removeChild(e.children[0]);
  }

  // <ul class="nav nav-list">
  //      <li><label class="tree-toggler nav-header">Header 1</label>

  var root = dali.stage.getRootLayer() ; // dali.stage.rootRotationActor;

  var recurse = function(parentElement, actor) {
    var children = actor.getChildren();
    if (children) {
      var ul = null;

      for (var i = 0; i < children.length; ++i) {
        var child = children[i];

        if (child.name[0] !== "*") {

          var li = document.createElement("li");

          var a = document.createElement("a");
          a.href = "javascript:selectActor(" + child.getId() + ");";
          a.text = "(" + child.getId() + ") '" + child.name + "'";
          li.appendChild(a);

          if (ul === null) {
            ul = document.createElement("ul");
            ul.className = "nav-tree";
          }

          ul.appendChild(li);

          recurse(li, child);

          // finish with the child wrapper
          child.delete();
        }
      }

      if (ul) {
        parentElement.appendChild(ul);
      }
    }
  };

  recurse(e, root);

  root.delete(); // wrapper
}


/**
 * Eventhandler class; Touch handling abstraction.
 *
 * handling functions can be registered to get touch events
 */
function EventHandler() {
  "use strict";
  // public
  this.mouseX = 0;
  this.mouseY = 0;
  this.mouse_buttons = 0;
  this.mouseDownPosition = [0, 0, 0];
  this.dragDx = 0;
  this.dragDy = 0;
  this.touchedActor = null; // set if actor is touched (left click)
  //private
  // this.selectedActor = null;
  this.mouseIsDown = 0;
  this.metaKey = 0;
  this.altKey = 0;
  this.ctrlKey = 0;
  this.shiftKey = 0;
  // handlers; use register("MouseDragStart", func); where for func(eventHandler)
  this.handlersMouseMove = []; // valid; touchedActor(can be null), mouseX, mouseY
  this.handlersMouseDrag = []; // valid; touchedActor(can be null), dragDx, dragDy
  this.handlersSelect = []; // valid; touchedActor(can be null), dragDx, dragDy
  this.handlersDeselect = []; // valid; touchedActor(can be null), dragDx, dragDy
}

/*
 * Register a handler, ie register("MouseMove", moveFunc)
 */
EventHandler.prototype.register = function(name, handler) {
  "use strict";
  var handlers = this["handlers" + name];
  if (!(handler in handlers)) {
    handlers.push(handler);
  }
};

/*
 * Unregister a handler
 */
EventHandler.prototype.unRegister = function(name, handler) {
  "use strict";
  var handlers = this["handlers" + name];
  var index = handlers.index(handler);
  if (index >= 0) {
    handlers.splice(index, 1);
  }
};

/*
 * update the mouse state
 */
EventHandler.prototype.updateMouseState = function(canvas, e) {
  "use strict";
  var rect = canvas.getBoundingClientRect(); // in window
  this.mouseX = e.clientX - rect.left; // from window
  this.mouseY = e.clientY - (rect.bottom - rect.height);
  this.mouse_buttons = e.button; //  0|1|2 left|middle|rigtht
};

/*
 * mouse down
 */
EventHandler.prototype.mouseDown = function(canvas, e) {
  "use strict";
  this.mouseIsDown = 1;
  this.updateMouseState(canvas, e);
  this.mouseDownPosition = [this.mouseX, this.mouseY, 0];
  var hitActor = dali.hitTest(this.mouseX, this.mouseY);
  var tid = -1;
  var hid = -2;
  if (this.touchedActor) {
    tid = this.touchedActor.getId();
  }
  if (hitActor) {
    hid = hitActor.getId();
  }

  if (hid !== tid) {
    if (tid > 0) {
      this.handleEvent(this.handlersDeselect);
      this.touchedActor = null;
    }
  }

  this.selectActor(hitActor);

  // dali is 0==down,1==up,2==motion
  dali.sendMouseEvent(this.mouseX, this.mouseY, 0);
  // console.log("mouseDown", this.mouseX, this.mouseY, this.mouseIsDown, 0);
};

/*
 * set the touchedActor
 */
EventHandler.prototype.selectActor = function(hitActor) {
  "use strict";
  if (hitActor) {
    this.handleEvent(this.handlersDeselect);

    var layer = dali.stage.getRootLayer();
    if (hitActor.getId() === dali.stage.rootRotationActor.getId()) {
      // dont select our rotation actor
      hitActor = null;
    } else {
      // dont select the root layer
      if (hitActor.getId() === layer.getId()) {
        hitActor = null;
      }
    }
    layer.delete(); // wrapper

    this.touchedActor = hitActor;
  } else {
    if(this.touchedActor) {
      this.handleEvent(this.handlersDeselect);
    }
    this.touchedActor = null;
  }

  this.handleEvent(this.handlersSelect);
};

/*
 * canvas mouse move handler
 */
EventHandler.prototype.mouseMove = function(canvas, e) {
  "use strict";
  this.updateMouseState(canvas, e);

  if(this.mouseIsDown) {
    // if (this.touchedActor) {
    this.dragDx = (this.mouseX - this.mouseDownPosition[0]);
    this.dragDy = (this.mouseY - this.mouseDownPosition[1]);
    // }

    this.handleEvent(this.handlersMouseDrag);
  }

  this.handleEvent(this.handlersMouseMove);

  // dali is 0==down,1==up,2==motion
  dali.sendMouseEvent(this.mouseX, this.mouseY, 2);
  // console.log("mouseMove", this.mouseX, this.mouseY, this.mouseIsDown, 2);
};

/*
 * canvas mouse up handler
 */
EventHandler.prototype.mouseUp = function(canvas, e) {
  "use strict";
  this.mouseIsDown = 0;
  this.updateMouseState(canvas, e);
  this.dragDx = (this.mouseX - this.mouseDownPosition[0]);
  this.dragDy = (this.mouseY - this.mouseDownPosition[1]);

  // dali is 0==down,1==up,2==motion
  dali.sendMouseEvent(this.mouseX, this.mouseY, 1);
  // console.log("mouseUp", this.mouseX, this.mouseY, this.mouseIsDown, 1);

};


/*
 * canvas mouse wheel handler
 */
EventHandler.prototype.mouseWheel = function(canvas, e) {
  "use strict";
  // multiples of +120 for up, -120 for down
  var clicks = e.wheelDelta / 120;

  var taskList = dali.stage.getRenderTaskList();
  var task = taskList.getTask(0);
  var cameraActor = task.getCameraActor();

  var fov = cameraActor.fieldOfView;
  cameraActor.fieldOfView = fov + (clicks / 180.0 * Math.PI);

  taskList.delete();
  task.delete();
  cameraActor.delete();
  // console.log(e.wheelDelta);
};

/*
 * call given handlers
 */
EventHandler.prototype.handleEvent = function(handlers) {
  for (var i = 0, len = handlers.length; i < len; i++) {
    var handler = handlers[i];
    handler(this);
  }
};

/*
 * canvas key down handler
 */
EventHandler.prototype.keyDown = function(canvas, e) {
  this.metaKey = e.metaKey;
  this.altKey = e.altKey;
  this.ctrlKey = e.ctrlKey;
  this.shiftKey = e.shiftKey;
};

/*
 * canvas key up handler
 */
EventHandler.prototype.keyUp = function(canvas, e) {
  this.metaKey = e.metaKey;
  this.altKey = e.altKey;
  this.ctrlKey = e.ctrlKey;
  this.shiftKey = e.shiftKey;
};

//
// Actor list helpers; applies function to all actors in the list
//
var ActorList = {
  doAll: function(actors, func) {
    for (var i = 0, len = actors.length; i < len; i++) {
      func(actors[i]);
    }
  },
  show: function(actors, visible) {
    this.doAll(actors, function(actor) {
      actor.visible = visible;
    });
  },
  delete: function(actors, visible) {
    this.doAll(actors, function(actor) {
      actor.delete = visible;
    });
  },
  add: function(actors, actor) {
    this.doAll(actors, function(a) {
      actor.add(a);
    });
  },
  remove: function(actors, actor) {
    this.doAll(actors, function(a) {
      actor.remove(a);
    });
  },
  contains: function(actors, actor) {
    if (!actor) {
      return false;
    }
    for (var i = 0, len = actors.length; i < len; i++) {
      if (actors[i].getId() == actor.getId()) {
        return true;
      }
    }
    return false;
  }
};


//
// Class for depth adornments
//
function DepthAdornment() {
  "use strict";
  this.attachedTo = null;
  this.actors = []; // array of 5 square attaced around and to back of actor to show depth

  for (var i = 0; i < 5; i++) { //

    var a = dali.createSolidColorActor([0.5, 0.5, 0.5, 1],
                                       false, [0, 0, 0, 1],
                                       0);
    a.name = "**_dali_adornment";
    this.actors.push(a);
  }

  ActorList.show(this.actors, false);
}

DepthAdornment.prototype.delete = function() {
  "use strict";
  ActorList.delete(this.actors);
  this.actors = [];
};

DepthAdornment.prototype.attach = function(actor) {
  "use strict";
  var newActor = true;
  if (this.attachedTo) {
    newActor = (actor.getId() !== this.attachedTo.getId());
  }

  if (actor && newActor) {
    this.unattach(this.attachedTo);

    if(!ActorList.contains(this.actors, actor)) {
      this.__positionActors(actor);

      ActorList.add(this.actors, actor);

      ActorList.show(this.actors, true);

      this.attachedTo = actor;
    }
  }
};

DepthAdornment.prototype.unattach = function() {
  "use strict";
  if (this.attachedTo) {
    ActorList.show(this.actors, false);
    ActorList.remove(this.actors, this.attachedTo);
    this.attachedTo = null;
  }
};

DepthAdornment.prototype.reposition = function() {
  "use strict";
  if (this.attachedTo) {
    this.__positionActors(this.attachedTo);
  }
};

DepthAdornment.prototype.__positionActors = function(actor) {
  "use strict";
  var s = actor.size;
  s[2] = Math.max(s[2], 20); // pretend the actor is at least 20
  var halfw = s[0] / 2;
  var halfh = s[1] / 2;
  var halfd = s[2] / 2;

  this.actors[0].position = [-halfw, 0, -halfd];
  this.actors[0].orientation = [0, -90, 0];
  this.actors[0].size = [s[2], s[1], 1];

  this.actors[1].position = [+halfw, 0, -halfd];
  this.actors[1].orientation = [0, 90, 0];
  this.actors[1].size = [s[2], s[1], 1];

  this.actors[2].position = [0, -halfh, -halfd];
  this.actors[2].orientation = [90, 0, 0];
  this.actors[2].size = [s[0], s[2], 1];

  this.actors[3].position = [0, halfh, -halfd];
  this.actors[3].orientation = [-90, 0, 0];
  this.actors[3].size = [s[0], s[2], 1];
  // back
  // this.actors[4].position = [0, 0, -s[2]];
  // this.actors[4].orientation = [0, 0, 0];
  // this.actors[4].size = s;

};



/**
 * Class for cursor adornments when displaying selected actor
 */
function Cursors() {
  "use strict";
  var i;
  var a;
  this.attachedTo = null;
  this.resizers = []; // array of 8 small square resizers
  this.lines = []; // array of 4 bounding box lines
  this.lineColor = [0.5, 0.5, 0.5, 1];
  this.resizerColor = [0, 0, 0, 1];
  this.depthAdornments = new DepthAdornment();

  for (i = 0; i < 4; i++) { // lines
    a = dali.createSolidColorActor(this.lineColor, false, [0, 0, 0, 1], 0);
    a.name = "**_dali_adornment";
    this.lines.push(a);
  }

  for(i = 0; i < 8; i++) { // squares
    a = dali.createSolidColorActor(this.resizerColor, true, [1, 1, 1, 1], 1);
    a.name = "**_dali_adornment";
    this.resizers.push(a);
    this.resizers[i].size = [10, 10, 1];
  }

  ActorList.show(this.lines, false);
  ActorList.show(this.resizers, false);

}


Cursors.prototype.attach = function(actor) {
  "use strict";
  if (actor) {

    if (this.attachedTo) {
      this.unattach(this.attachedTo);
    }

    this.__positionActors(actor);

    ActorList.add(this.lines, actor);
    ActorList.add(this.resizers, actor);

    ActorList.show(this.lines, true);
    ActorList.show(this.resizers, true);

    this.depthAdornments.attach(actor);

    this.attachedTo = actor;
  }
};

Cursors.prototype.unattach = function(actor) {
  "use strict";
  if (actor && this.attachedTo) {
    ActorList.show(this.lines, false);
    ActorList.show(this.resizers, false);

    ActorList.remove(this.lines, actor);
    ActorList.remove(this.resizers, actor);

    this.depthAdornments.unattach(actor);

    this.attachedTo = null;
  }
};

Cursors.prototype.reposition = function() {
  "use strict";
  if (this.attachedTo) {
    this.__positionActors(this.attachedTo);
    this.depthAdornments.reposition();
  }
};

Cursors.prototype.__positionActors = function(actor) {
  "use strict";
  var s = actor.size;
  var p = actor.position;
  var halfw = s[0] / 2;
  var halfh = s[1] / 2;

  this.lines[0].position = [0 - halfw, 0, 0];
  this.lines[0].size = [1, halfh * 2, 1];

  this.lines[1].position = [0, 0 - halfh, 0];
  this.lines[1].size = [halfw * 2, 1, 1];

  this.lines[2].position = [0 + halfw, 0, 0];
  this.lines[2].size = [1, halfh * 2, 1];

  this.lines[3].position = [0, 0 + halfh, 0];
  this.lines[3].size = [halfw * 2, 1, 1];

  // drag functions presumes order here is clockwise from top left
  var offsetZ = 5;
  this.resizers[0].position = [-halfw, +halfh, offsetZ];
  this.resizers[1].position = [0, +halfh, offsetZ];
  this.resizers[2].position = [+halfw, +halfh, offsetZ];
  this.resizers[3].position = [+halfw, 0, offsetZ];

  this.resizers[4].position = [+halfw, -halfh, offsetZ];
  this.resizers[5].position = [0, -halfh, offsetZ];
  this.resizers[6].position = [-halfw, -halfh, offsetZ];
  this.resizers[7].position = [-halfw, 0, offsetZ];
};



function App(theEventHandler) {
  "use strict";
  var self = this;
  this.rootRotation = [0, 0, 0]; // in degrees
  this.cursors = new Cursors();
  // ActorWrapper CreateSolidColorActor(const Vector4 & color, bool border,
  //   const Vector4 & borderColor,
  //     const unsigned int borderSize)
  this.rootLayerIndicator = dali.createSolidColorActor(
    [0, 0, 0, 0.0],
    true, [0, 0, 0, 1],
    1);
  this.rootLayerIndicator.name = "**_dali_adornment";

  this.rootLayerIndicator.parentOrigin = [0.5, 0.5, 0.5];
  this.rootLayerIndicator.anchorPoint = [0.5, 0.5, 0.5];
  this.rootLayerIndicator.size = [250, 250, 1];
  this.rootLayerIndicator.position = [0, 0, 0];

  this.selectedActor = null;
  this.selectedActorSize = [0, 0, 0];
  this.selectedActorPosition = [0, 0, 0];

  this.__handleMouseDrag = function(eventHandler) {
    if (self.resizing) {
      if (self.selectedActor) {
        self.selectedActor.size = [
          self.selectedActorSize[0] + eventHandler.dragDx,
          self.selectedActorSize[1] + eventHandler.dragDy,
          self.selectedActorSize[2]
        ];
        self.cursors.reposition();
      }
    } else {
      // move the touched actor
      if (self.selectedActor) {
        if (eventHandler.mouse_buttons === 0) {
          self.selectedActor.position = [
            self.selectedActorPosition[0] + eventHandler.dragDx,
            self.selectedActorPosition[1] + eventHandler.dragDy,
            self.selectedActorPosition[2]
          ];
        } else if (eventHandler.mouse_buttons === 2) {
          self.selectedActor.position = [
            self.selectedActorPosition[0],
            self.selectedActorPosition[1], -(self.selectedActorPosition[2] + eventHandler.dragDy)
          ];
        }
      }
    }

    // rotate on middle button
    if (eventHandler.mouse_buttons === 1) {
      self.rootRotation = [eventHandler.mouseDownPosition[1] - eventHandler.mouseY, -(eventHandler.mouseDownPosition[0] - eventHandler.mouseX),
                           self.rootRotation[2]
                          ];
      dali.stage.rootRotationActor.orientation = self.rootRotation;
    }

  };

  this.addActor = function(actor) {
    if (eventHandler.touchedActor) {
      eventHandler.touchedActor.add(actor);
      actor.position = [20,20,0]; // offset so new actor isnt right behind touched
    } else {
      dali.stage.add(actor);
    }

    rebuildTree();

  };

  this.__handleSelect = function(eventHandler) {
    if (eventHandler.mouse_buttons === 0) { // left click only selects actor
      var actor = eventHandler.touchedActor;
      self.selectedActor = actor;
      if(actor) {
        self.selectedActorPosition = actor.position;
        self.selectedActorSize = actor.size;
      } else {
        self.selectedActorPosition = [0, 0, 0];
        self.selectedActorSize = [1, 1, 1];
      }
      self.cursors.attach(self.selectedActor);
      dali.stage.add(self.rootLayerIndicator);
      onActorSelected(actor);
    }
  };

  this.__handleDeselect = function(eventHandler) {
    self.cursors.unattach(self.selectedActor);
    dali.stage.remove(self.rootLayerIndicator);
  };

  // initialize
  eventHandler.register("MouseDrag", this.__handleMouseDrag);
  eventHandler.register("Select", this.__handleSelect);
  eventHandler.register("Deselect", this.__handleDeselect);

  //
  // Monkey patch the stage object (to add to root object not the root layer for rotations)
  //
  dali.stage.rootRotationActor = new dali.Actor();
  dali.stage.rootRotationActor.parentOrigin = [0.5, 0.5, 0.5];
  dali.stage.rootRotationActor.anchorPoint = [0.5, 0.5, 0.5];
  dali.stage.rootRotationActor.name = "app rotation actor";
  dali.stage.rootRotationActor.size = [100,100,1];
  dali.stage.add(dali.stage.rootRotationActor);

  dali.stage.add = function(actor) {
    dali.stage.rootRotationActor.add(actor);
  };

  dali.stage.remove = function(actor) {
    dali.stage.rootRotationActor.remove(actor);
  };

}

//
// Global
//

var eventHandler = new EventHandler();
var app = new App(eventHandler);
var canvas = document.getElementById("canvas");

function mouseDown(e) {
  "use strict";
  eventHandler.mouseDown(canvas, e);
}

function mouseUp(e) {
  "use strict";
  eventHandler.mouseUp(canvas, e);
}

function mouseMove(e) {
  "use strict";
  eventHandler.mouseMove(canvas, e);
}

function mouseWheel(e) {
  "use strict";
  eventHandler.mouseWheel(canvas, e);
}

function keyDown(e) {
  "use strict";
  eventHandler.keyDown(canvas, e);
}

function keyUp(e) {
  "use strict";
  eventHandler.keyUp(canvas, e);
}

canvas.onmousemove = mouseMove;
canvas.onmousedown = mouseDown;
canvas.onmouseup = mouseUp;
canvas.onwheel = mouseWheel;

canvas.onkeydown = keyDown;
canvas.onkeyup = keyUp;

//
//
//

function runRandomAnimation(actor) {
  "use strict";
  var a = new dali.Animation(0);
  a.setDuration(6);
  a.setLooping(true);
  a.animateTo(actor, "position", [300 * Math.random() - 150, 300 * Math.random() - 150, 0],
              "Linear", 0, 3);
  a.animateTo(actor, "position", [0, 0, 0], "Linear", 3, 3);
  a.animateTo(actor, "rotation", [0, 0, 1, 720 * Math.random() - 360], "Linear", 0, 3);
  a.animateTo(actor, "rotation", [0, 0, 1, 0], "Linear", 3, 3);

  a.play();
  //    a.delete(); // delete the animtion object but a.Play() keeps it running in Dali
  return a;
}


//------------------------------------------------------------------------------
//
// application building helpers (pseudo DSL)
//
//------------------------------------------------------------------------------
var _whenBuilder = { // builder for when() function. mimicks dali builder
  paths: {},
  styles: {},
  templates: {},
  propertyBuffers: {},
  geometries: {},
  imageBuffers: {},
  materials: {},
  samplers: {},
  shaders: {},
  renderers: {},
  tags: {}
};

function _actorId(a) { // return id from [object|string name|id]
  "use strict";
  if( typeof a === "object" ) {
    return a.getId();
  } else if( typeof a === "string" ) {
    var child = dali.stage.findChildByName(a);
    return child.getId();
  } else {
    return a;
  }
}

/**
 * Builder namespace. Usage as singleton "build".
 */
dali.Builder = function() {
  "use strict";
  this.data = _whenBuilder;
  this.init();
};

/**
 * 
 */
dali.Builder.prototype.init = function() {
  var imageSize = 0.5; // half quad size
  this.propertyBuffer("imageVerts",
                       {format: [ ["aPosition", dali.PropertyType.VECTOR3],
                                  ["aTexCoord", dali.PropertyType.VECTOR2] ],
                        data: { "aPosition": [ [-imageSize, -imageSize, 0.0],
                                               [+imageSize, -imageSize, 0.0],
                                               [-imageSize, +imageSize, 0.0],
                                               [+imageSize, +imageSize, 0.0]
                                             ],
                                "aTexCoord": [ [0, 0],
                                               [1, 0],
                                               [0, 1],
                                               [1, 1]
                                             ]
                              }
                       }
                      );

  this.propertyBuffer("imageIndices",
                      { format: [ ["indices", dali.PropertyType.INTEGER]],
                        data: { "indices": [0, 3, 1, 0, 2, 3] } } );

  this.geometry("imageGeometry", {vertices: ["imageVerts"],
                                  indices: "imageIndices",
                                  type: "TRIANGLES",
                                  requiresSelfDepthTest: false} );

  this.shader("imageShader",
              {vertex: "\n" +
               "attribute mediump vec3 aPosition;\n" +
               "attribute mediump vec2 aTexCoord;\n" +
               "varying mediump vec2 vTexCoord;\n" +
               "uniform mediump vec3 uSize;\n" +
               "uniform mediump mat4 uMvpMatrix;\n" +
               "\n" +
               "void main(void)\n" +
               "{\n" +
               "  mediump vec4 vertexPosition = vec4(aPosition, 1.0);\n" +
               "  vertexPosition.xyz *= uSize;\n" +
               "  gl_Position = uMvpMatrix * vertexPosition;\n" +
               "  vTexCoord = aTexCoord;\n" +
               "}\n" +
               "\n" +
               "",
               fragment:
               "precision mediump float;\n" +
               "uniform sampler2D sTexture;\n" +
               "uniform mediump vec4 uColor;\n" +
               "varying mediump vec2 vTexCoord;\n" +
               "\n" +
               "void main()\n" +
               "{\n" +
               "  gl_FragColor =  texture2D( sTexture, vTexCoord ) * uColor;\n" +
               "}\n" +
               "\n" +
               "",
               hints: "HINTS_NONE"
              });

  var builtInImages = ["girl1","funnyface","ducks","field"];

  var d;
  for(var i=0; i < builtInImages.length; i++)
  {
    var imageName = builtInImages[i];

    var samplerName = "sampler_" +  imageName;
    this.sampler(samplerName,
                 { "minification-filter":0,
                   "magnification-filter":0,
                   "u-wrap":0,
                   "v-wrap":0,
                   "affects-transparency":false});

    var materialName ="material_" +  imageName;
    this.material(materialName,
                  {textures:
                   [
                     { image: imageName,
                       uniform: "sTexture",
                       sampler: samplerName
                     }
                   ],
                   shader: "imageShader"} );

    this.renderer("render_" + imageName,
                  {geometry: "imageGeometry",
                   material: materialName});

  }

}; // init()

/**
 * tags an actor
 */
dali.Builder.prototype.tag = function(actor, tag) {
  // tag an actor (multiple by seperated by space)
  "use strict";
  var names = tag.split();
  var id = _actorId(actor);
  for(var i = 0; i < names.length; i++) {
    var name = names[i];
    if(!(name in _whenBuilder.tags)) {
      _whenBuilder.tags[name] = [];
    }
    var nameIndex = _whenBuilder.tags[name].indexOf(id);
    if(nameIndex < 0) {
      _whenBuilder.tags[name].push( id );
    }
  }
};

/**
 * unTags an actor
 */
dali.Builder.prototype.unTag = function(actor, tag) {
  // untag
  "use strict";
  var names = tag.split();
  var id = _actorId(actor);
  for(var i = 0; i < names.length; i++) {
    var name = names[i];
    if( name in _whenBuilder.tags ) {
      var nameIndex = _whenBuilder.tags[name].indexOf(id);
      if(nameIndex >= 0) {
        _whenBuilder.tags[name].splice(nameIndex, 1);
      }
    }
  }
};

/**
 * Gets list of actors tagged by 'tag'
 */
dali.Builder.prototype.tagged = function(tag) {
  // return all tagged
  "use strict";
  return _whenBuilder.tags[tag];
};

/**
 * Returns actorIdArray with actorOrArray removed
 */
dali.Builder.prototype.excludeFrom = function(actorIdArray, actorOrArray)
{
  // return all except one
  "use strict";
  var item, id;
  var excludes = {};
  if("length" in actorOrArray) {
    for(item in actorOrArray) {
      id = _actorId(item);
      excludes[id] = 1;
    }
  } else {
    id = _actorId(actorOrArray);
    excludes[id] = 1;
  }

  var ret = [];
  for(item in actorIdArray) {
    if( !(item in excludes) ) {
      ret.push( item );
    }
  }
  return ret;
};

/**
 * 
 */
dali.Builder.prototype.path = function (d) {
  // Stores paths in builder
  // paths({name: {points: [[1,2,3],[4,5,6]], forward:[1,0,0]},
  //        other:{points: [[1,2,3],[4,5,6]], forward:[1,0,0]}})
  //
  "use strict";
  for(var name in d) {
    var pathData = {point: [], forward: [1, 0, 0]};
    pathData = dali.mergeObjects(pathData, d[name]);
    var daliPath = new dali.Path();
    daliPath.points = pathData.points;
    dali.generateControlPoints(daliPath, pathData.curvature); // @todo remove magic number?
    _whenBuilder.paths[name] = daliPath;
  }
};

/**
 * Set style definition
 */
dali.Builder.prototype.style = function(name, d) {
  // Stores styles in builder
  // style(name, {property1:value, prop2,value})
  // style(other, {property1:value, prop2,value})
  //
  "use strict";
  _whenBuilder.styles[name] = d;
};

//
//  +------------------+          +---------------+
//  | PropertyBuffer   | *      * | Geometry      |
//  +------------------+ -------- +---------------+
//  | format:          |          | vertex:PBuf   |1      * +-----------+         +------------+
//  |   name:v2,name:v3|          | indices:PBuf  +---------| Renderer  |         | Actor      |
//  | data             |          |               |         +-----------+ *     * +------------+
//  +------------------+          +---------------+       * | geom      +---------| renderer(s)|
//                                                     +----| material  |         |            |
//  +-------------+ *      * +----------+ 1            |    |           |         |            |
//  |Sampler      +----------|Material  +--------------+    +-----------+         +------------+
//  +-------------+          +----------+
//  |uniformName  |        * |sampler(s)|
//  |Image        |       +--|shader    |
//  |Filter/wrap  |       |  |facecull  |
//  +-------------+       |  |blendfunc |
//                        |  +----------+
//  +-----------------+   |
//  |Shader           |   |
//  +-----------------+ 1 |
//  |program:vert,frag|---+
//  |hints            |
//  +-----------------+
//

/**
 * Set template defintion
 */
dali.Builder.prototype.template = function(name, d) {
  // Stores templates in builder
  // template("name":{ "type": "ImageActor", property1:value, prop2,value})
  // template("other: { "type": "TextView", property1:value, prop2,value})
  "use strict";
  _whenBuilder.templates[name] = d;

};

/**
 * Set style definition
 */
dali.Builder.prototype.imageBuffer = function(name, d) {
  // stores imageBuffers
  // imageBuffers("buffer1", { bufferName: "" }
  //                         } )
  "use strict";
  _whenBuilder.imageBuffers[name] = d;
};

/**
 * Set property buffer definition
 */
dali.Builder.prototype.propertyBuffer = function(name, d) {
  // stores PropertyBuffers
  // propertyBuffers("buffer1", { format: [ ["aPosition", dali.PropertyType.VECTOR2],
  //                                       ["aTexCoord", dali.PropertyType.VECTOR2].
  //                             data: {aPosition: [-0.5,-0.5,],
  //                                               [+0.5,-0.5,],
  //                                               [-0.5,+0.5,],
  //                                               [+0.5,+0.5,],
  //                                    aCol:      [0.0, 0.0],
  //                                               [1.0, 0.0],
  //                                               [0.0, 1.0],
  //                                               [1.0, 1.0] }
  //                           } )
  "use strict";
  _whenBuilder.propertyBuffers[name] = d;
};

/**
 * Set geometry definition
 */
dali.Builder.prototype.geometry = function(name, d) {
  // stores geometry meshes
  // geometries("mesh1": { vertices: "buffer1",
  //                       index: "index1",
  //                       type: "TRIANGLES",
  //                       requresDepthTest:false } )
  "use strict";
  _whenBuilder.geometries[name] = d;
};

/**
 * Set sample definition
 */
dali.Builder.prototype.sampler = function(name, d) {
  // stores samplers
  // samplers("sampler1", { "minification-filter":0,
  //                       "magnification-filter":0,
  //                       "u-wrap":0,
  //                       "v-wrap":0,
  //                       "affects-transparency":false})
  "use strict";
  _whenBuilder.samplers[name] = d;
};

/**
 * Set shader definition
 */
dali.Builder.prototype.shader = function(name, d) {
  // stores shaders
  // shaders("shader1", { vertex:"",
  //                     fragment:"",
  //                     hints:"NONE"} )
  "use strict";
  _whenBuilder.shaders[name] = d;
};

/**
 * Set material definition
 */
dali.Builder.prototype.material = function(name, d) {
  // stores materials meshes
  // materials({"mat1",{ textures: [
  //                       { image: animage,
  //                         uniform: "uTexture"
  //                         sampler: asampler
  //                        }
  //                    ],
  //                    shader: "shader1",
  //                    faceCulling: "BACK",
  //                    blend: "AUTO",
  //                    blendFunc : {srcFactorRGBA, destFactorRGBA},
  //                    blendEquation : "",
  //                    mixColor : [1,0,0,1] } )
  //
  "use strict";
  var value; // check required things
  value = this.GetRequiredKey(d, "shader");
  this.GetRequiredKey(_whenBuilder.shaders, value);  // must already exist

  // only support textures so must have textures section
  this.GetRequiredKey(d, "textures");

  var i;
  for(i = 0; i < d.textures.length; i++)
  {
    if(typeof(d.textures[i]) == "string")
    {
      // the image must already exist
      value = this.GetRequiredKey(d.textures[i], "image");
      this.GetRequiredKey(_whenBuilder.imageBuffers, value);
    }
    else
    {
    }
  }

  // value = this.GetRequiredKey(d, "textures");
  // this.GetRequiredKey(_whenBuilder.textures, value);
  _whenBuilder.materials[name] = d;
};

/**
 * Set renderer definition
 */
dali.Builder.prototype.renderer = function(name, d) {
  // store renderer
  // renderers("rend1",  {"geometry": "geom1",
  //                     "material": "mat1",
  //                     "depth-index"})
  "use strict";
  var value; // check required things
  value = this.GetRequiredKey(d, "geometry");
  this.GetRequiredKey(_whenBuilder.geometries, value);  // must already exist
  value = this.GetRequiredKey(d, "material");
  this.GetRequiredKey(_whenBuilder.materials, value);

  _whenBuilder.renderers[name] = d;
};

dali.Builder.prototype.createOrGetDaliPropertyBuffer = function(name) {
  "use strict";
  consoleAssert(name in _whenBuilder.propertyBuffers, "property buffer not defined:" + name);
  var ret;
  if("daliObject" in _whenBuilder.propertyBuffers[name]) {
    ret = _whenBuilder.propertyBuffers[name].daliObject;
  } else {
    var data = _whenBuilder.propertyBuffers[name];

    ret = dali.createPropertyBuffer(data);

    _whenBuilder.propertyBuffers[name].daliObject = ret;
  }
  return ret;
};

dali.Builder.prototype.createOrGetDaliGeometry = function(name) {
  "use strict";
  consoleAssert(name in _whenBuilder.geometries, "geometry not defined:" + name);
  var ret;
  if("daliObject" in _whenBuilder.geometries[name]) {
    ret = _whenBuilder.geometries[name].daliObject;
  } else {
    var data = _whenBuilder.geometries[name];

    ret = new dali.Geometry();

    for(var i = 0; i < data.vertices.length; i++) {
      ret.addVertexBuffer( this.createOrGetDaliPropertyBuffer(data.vertices[i] ) );
    }

    ret.setIndexBuffer( this.createOrGetDaliPropertyBuffer(data.indices) );

    if("requiresSelfDepthTest" in data) {
      ret.setRequiresDepthTesting(data.requiresSelfDepthTest);
    }

    _whenBuilder.geometries[name].daliObject = ret;
  }
  return ret;
};

dali.Builder.prototype.createOrGetDaliShader = function(name) {
  "use strict";
  consoleAssert(name in _whenBuilder.shaders, "shader not defined:" + name);
  var ret = null;
  if("daliObject" in _whenBuilder.shaders[name]) {
    ret = _whenBuilder.shaders[name].daliObject;
  } else {
    var data = _whenBuilder.shaders[name];

    ret = new dali.Shader(data.vertex,
                          data.fragment,
                          uiShaderTab.getDaliShaderHints(data.hints));

    _whenBuilder.shaders[name].daliObject = ret;
  }

  return ret;
};

dali.Builder.prototype.createOrGetDaliImage = function(name) {
  "use strict";
  if(!(name in _whenBuilder.imageBuffers)) {
    _whenBuilder.imageBuffers[name] = {};
  }

  var ret;
  if("daliObject" in _whenBuilder.imageBuffers[name]) {
    ret = _whenBuilder.imageBuffers[name].daliObject;
  } else {
    // get images from the ui buffer
    ret = imageFromUiBuffer(name);
    _whenBuilder.imageBuffers[name].daliObject = ret;
  }
  return ret;
};

dali.Builder.prototype.createOrGetDaliSampler = function(name) {
  "use strict";
  consoleAssert( name in _whenBuilder.samplers, "sampler not defined:" + name);
  var ret;
  if("daliObject" in _whenBuilder.samplers[name]) {
    ret = _whenBuilder.samplers[name].daliObject;
  } else {
    var data = _whenBuilder.samplers[name];
    ret = new dali.Sampler();
    _whenBuilder.samplers[name].daliObject = ret;
  }
  return ret;
};

dali.Builder.prototype.createOrGetDaliMaterial = function(name) {
  "use strict";
  consoleAssert( name in _whenBuilder.materials, "Material not defined:" + name);
  var ret;
  if("daliObject" in _whenBuilder.materials[name]) {
    ret = _whenBuilder.materials[name].daliObject;
  } else {
    var data = _whenBuilder.materials[name];

    ret = new dali.Material(this.createOrGetDaliShader( data.shader ));

    for(var i = 0; i < data.textures.length; i++) {
      ret.addTexture( this.createOrGetDaliImage(data.textures[i].image),
                      data.textures[i].uniform,
                      this.createOrGetDaliSampler( data.textures[i].sampler ) );
    }

    _whenBuilder.materials[name].daliObject = ret;
  }
  return ret;
};

dali.Builder.prototype.createOrGetDaliRenderer = function(name) {
  "use strict";
  consoleAssert( name in _whenBuilder.renderers, "Renderer not defined:" + name);

  var ret;
  if("daliObject" in _whenBuilder.renderers[name]) {
    ret = _whenBuilder.renderers[name].daliObject;
  } else {
    var data = _whenBuilder.renderers[name];

    ret = new dali.Renderer(this.createOrGetDaliGeometry(data.geometry),
                            this.createOrGetDaliMaterial(data.material));

    _whenBuilder.renderers[name].daliObject = ret;
  }
  return ret;
};

dali.Builder.prototype.createActorTree = function(dictionary) {
  "use strict";
  var ret;
  var i;
  consoleAssert(typeof(dictionary) == "object", "createActorTree needs an object");

  if("type" in dictionary) {
    if( dictionary.type in dali ) {
      ret = new dali[ dictionary.type ]();
    } else if( dictionary.type in _whenBuilder.templates ) {
      var realType = _whenBuilder.templates[ dictionary.type ].type;
      ret = new dali[ realType ]();
    } else {
      consoleAssert(0, "Cannot find type to create:" + dictionary.type);
    }
  } else {
    consoleAssert("render" in dictionary, "'type' or 'render needed to create dali object");
    ret = new dali.Actor();
  }

  var tags = [];

  for(var prop in dictionary) {
    if( prop === "actors" ) {
      var actors = dictionary[prop];
      for(i = 0; i < actors.length; i++) {
        ret.add( this.createActorTree( actors[i] ) );
      }
    } else if( prop === "tag" ) {
      tags.push(dictionary.tag);
    } else if( prop === "tags" ) {
      tags.concat(dictionary.tags);
    } else if( prop === "type" ) {
      // ignore
    } else if( prop === "render") {
      var renderer;
      if(typeof(dictionary.render) == "string")
      {
        renderer = this.createOrGetDaliRenderer(dictionary.render);
        ret.addRenderer(renderer);
      }
      else
      {
        for(i = 0; i < dictionary.render.length; i++) {
          renderer = this.createOrGetDaliRenderer(dictionary.render[i]);
          ret.addRenderer(renderer);
        }
      }
    } else {
      // a property set
      ret[prop] = dictionary[prop];
    }
  }

  if(tags) {
    for(i = 0; i < tags.length; i++) {
      this.tag(ret, tags[i]);
    }
  }

  return ret;
};

/**
 * Adds actor defintions to stage.
 */
dali.Builder.prototype.stage = function(/* actor definition array */) {
  "use strict";
  for(var i = 0; i < arguments.length; i++) {
    dali.stage.add( this.createActorTree( arguments[i] ) );
  }
};

/**
 * Adds actor defintions to stage.
 */
dali.Builder.prototype.addTo = function(/* toActor, actor_definition_array */) {
  "use strict";
  consoleAssert(typeof(arguments[0]) == "string", "1st argument must be name");

  var i;

  for(i = 1; i < arguments.length; i++) {
    consoleAssert(typeof(arguments[1]) == "object", "2nd and other arguments must be object");
  }

  var actor;
  if( arguments[0] == "stage" ) {
    actor = dali.stage.getRootLayer();
  }
  else {
    actor = dali.stage.getRootLayer().findChildByName( arguments[0] );
    consoleAssert(actor, "Cannot find actor:" + arguments[0]);
  }

  for(i = 1; i < arguments.length; i++) {
    dali.stage.add( this.createActorTree( arguments[i] ) );
  }
};

/**
 * Adds actor defintions to stage.
 */
dali.Builder.prototype.GetRequiredKey = function(object, name, caller) {
  if( name in object )
  {
    return object[name];
  }
  else
  {
    consoleAssert(0, "Object is missing key:'" + name + "'\n" + JSON.stringify(object));
    return false;
  }
};

dali.Builder.prototype.image = function(dictionary) {
  // image({name:"name"
  //
  //       })
  //
  //
  "use strict";
  var d = {};
  var name = this.GetRequiredKey(dictionary, "name");

  var shaderName = "imageShader";
  if("shader" in dictionary) {
    shaderName = dictionary.shader;
  }
  var samplers = [name + "_sampler"];
  if("samplers" in dictionary) {
    samplers = dictionary.samplers;
  }

  var samplername = name + "_sampler";
  d = {};
  d[samplername] = {};
  this.samplers(d);

  var matname = name + "_material";
  d = {};
  d[matname] = {shader: shaderName, textures: [ {image: this.GetRequiredKey(dictionary, "image"),
                                                 uniform: "sTexture",
                                                 sampler: samplername}]};
  this.materials(d);

  var rendname = name + "_renderer";
  d = {};
  d[rendname] = {geometry: "imageGeometry", material: matname};
  this.renderers(d);

  // store imageBuffers if they are missing for image
  // (we're only using image data from browser buffers)
  if( !(dictionary.image in _whenBuilder.imageBuffers) ) { // add to _whenBuilder
    d = {};
    d[dictionary.image] = { bufferName: dictionary.image };
    this.imageBuffers(d);
  }

  dictionary.type = "Actor";
  dictionary.renderers = [rendname];

  return dictionary;
};

////////////////////////////////////////////////////////////////////////////////
//
// dali toy Builder singleton
//
//
////////////////////////////////////////////////////////////////////////////////
var build = new dali.Builder();

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
function _addTypeToDictionaryClosure(actorType) {
  "use strict";
  return function(dictionary) {
    dictionary.type = actorType;
    return dictionary;
  };
}

// @ todo; why is this global?
var HTML5Image = Image; // html5 also has "Image" which were about to overwrite
var _registry = new dali.TypeRegistry();
for(var _i = 0; _i < _registry.getTypeNameCount(); _i++) {
  var _typename = _registry.getTypeName(_i);
  this[_typename] = _addTypeToDictionaryClosure(_typename);
}

function _actorArray(actorOrName) {
  "use strict";
  if(typeof actorOrName === "string") {
    return [actorOrName];
  } else {
    return [actorOrName.name];
  }
}

function _actorIdList(a) {
  "use strict";
  var ret = [];
  if("length" in a) {
    for(var item in a) {
      ret.push( _actorId(item) );
    }
  } else {
    ret.push( _actorId(a) );
  }
  return ret;
}


function _functionName(fun) {
  "use strict";
  var ret = fun.toString();
  ret = ret.substr("function ".length);
  ret = ret.substr(0, ret.indexOf("("));
  return ret;
}

function _numberOrRaise(value, errorMessage) {
  "use strict";
  if(typeof value === "number") {
    return value;
  } else {
    throw errorMessage;
  }
}

function _stringOrRaise(value, errorMessage) {
  "use strict";
  if(typeof value === "string") {
    return value;
  } else {
    throw errorMessage;
  }
}

// Animate to
//   to(value)
//   to(value, delay, duration)
//   to(value, delay, duration, interpolation)
// ie
//   to([10,10,10]
//   to([10,10,10], 0,3, "ease_in")),
function to() {
  "use strict";
  var ret = { animate: "animateTo",
              value: null,
              delay: 0,
              duration: 0,
              alpha: "linear"};

  ret.value = arguments[0]; // property value
  if( arguments.length > 1 ) {
    ret.delay = _numberOrRaise(arguments[1], "Delay must be a number");
  }
  if( arguments.length > 2 ) {
    ret.duration = _numberOrRaise(arguments[2], "Duration must be a number");
  }
  if( arguments.length > 3 ) { // optional
    ret.alpha = arguments[3];
  }
  return ret;
}

// Animate by
//   by(value)
//   by(value, delay, duration)
//   by(value, delay, duration, interpolation)
// ie
//   by([10,10,10]
//   by([10,10,10], 0,3, "ease_in")),
function by() {
  "use strict";
  var ret = { animate: "animateBy",
              value: null,
              delay: 0,
              duration: 0,
              alpha: "linear"};

  ret.value = arguments[0]; // property value
  if( arguments.length > 1 ) {
    ret.delay = _numberOrRaise(arguments[1], "Delay must be a number");
  }
  if( arguments.length > 2 ) {
    ret.duration = _numberOrRaise(arguments[2], "Duration must be a number");
  }
  if( arguments.length > 3 ) { // optional
    ret.alpha = arguments[3];
  }
  return ret;
}

// Animate between
//   between(fromValue, toValue)
//   between(fromValue, toValue, delay, duration)
//   between(fromValue, toValue, delay, duration, interpolation)
// ie
//   between([10,10,10], [100,10,10]
//   between([10,10,10], [100,10,10], 0,3, "ease_in")),
function between() {
  "use strict";
  var ret = { animate: "animateBy",
              fromValue: null,
              toValue: null,
              delay: 0,
              duration: 0,
              alpha: "linear"};

  ret.value = arguments[0]; // property value
  ret.value = arguments[1]; // property value
  if( arguments.length > 2 ) {
    ret.delay = _numberOrRaise(arguments[2], "Delay must be a number");
  }
  if( arguments.length > 3 ) {
    ret.duration = _numberOrRaise(arguments[3], "Duration must be a number");
  }

  if( arguments.length > 4 ) { // optional
    ret.alpha = arguments[4];
  }
  return ret;
}

// Animate path
//   path(pathName)
//   path(pathName, delay, duration)
//   path(pathName, delay, duration, interpolation)
// ie
//   path("path0", 0, 3, "linear")
function path() {
  "use strict";
  var ret = { animate: "path",
              path: null,
              forward: [1, 0, 0],
              delay: 0,
              duration: 0,
              alpha: "linear"};

  ret.path = arguments[0]; // path
  if( arguments.length > 1 ) {
    ret.delay = _numberOrRaise(arguments[1], "Delay must be a number");
  }
  if( arguments.length > 2 ) {
    ret.duration = _numberOrRaise(arguments[2], "Duration must be a number");
  }

  if( arguments.length > 3 ) { // optional
    ret.alpha = arguments[3];
  }
  return ret;
}

// conditional animation
// when("myimage", condition("touched", "inside", 0, 100),
//
function condition(propertyName, conditionType, arg0, arg1)
{
  "use strict";
  var ret = { actors: [],
              property: propertyName,
              type: conditionType,
              arg0: arg0,
              arg1: arg1
            };

  if(arg1 === undefined) {
    ret.arg1 = 0.0; // make a float for the c++ call (ignored if irrelevant)
  }

  return ret;
}

// set animation
//   set(actorOrListOrNone, signalName, animationDefition)
// ie
//   set("myimage", "size", to([10,10,10], 0,3, "ease_in"),
//                          path(0,3, "ease_in", "path0"))
//   set("size", to([10,10,10], 0,3, "ease_in")),
//               between([0,0,0], [10,10,10], 0,3, "ease_in")))
//   set(tagged("scrollitem), "position", path(0,3, "ease_in", "path0")),
//   set(excludeFrom(tagged("scrollitem"), "myimage"), "position", path(0,3, "ease_in", "path0")),
function set() {
  "use strict";
  var ret = { action: "set",
              actors: [],
              property: null,
              animation: []
            };

  var index;
  if( arguments.length === 3 ) { // then actor is specified somehow
    ret.actors = _actorArray( arguments[0] );
    ret.property = _stringOrRaise(arguments[1], "Property must be a string");
    index = 2;
  } else if( arguments.length === 2 ) { // actor is self (indicated in 'when' call)
    ret.property = _stringOrRaise(arguments[0], "Property must be a string");
    index = 1;
  } else {
    throw "set needs 2 or 3 arguments";
  }

  for(var i = index; i < arguments.length; i++) {
    ret.animation.push( arguments[i] );
  }

  return ret;
}

// set animation
//   call(function, arg0, arg1)
// ie
//   call(func)
//   call(func, 10.0, "enable")
function call() {
  "use strict";
  var ret = { action: "call",
              arguments: [],
              func: null
            };

  ret.func = arguments[0];

  for(var i = 1; i < arguments.length; i++) {
    ret.arguments.push( arguments[i] );
  }

  return ret;
}

function then()
{
  "use strict";
  return [{ action: arguments[0],
            parameters: arguments[1]}];
}

function _actorNameList()
{
  "use strict";
  var names;
  if(typeof arguments[0] === "function") {
    var lookup = arguments[0]();
    names = lookup[0];
  } else if(typeof arguments[0] === "object") {
    // or it can be the actual dali actor object
    names = _actorArray(arguments[0].name);
  } else if(typeof arguments[0] === "string") {
    // or arg0,arg1 are the actor property
    names = _actorArray(arguments[0]);
  } else {
    return names; // presume already actor name list
  }
  return names;
}

var _buildDescription = {animations:{}, templates:{}};

function _animation(a) {
  if(a) {
    return a;
  } else {
    return new dali.Animation(0);
  }
}

// function _makeActionCallback(func, args) {
//   "use strict";
//   return function(){
//     var a;
//     for(var i = 0; i < args.length; i++) {
//       var d = args[i];
//       if( d.action === "set" ) {
//         for(var ai = 0; ai < d.actors.length; ai++) {
//           if( d.property ) {
//             apply(_animation,
//                   d.arguments[0].animate,

//                   anim.push( [d.actors[ai]], d.property,
//           }
//         }
//       }

//   var ret = { action: "set",
//               actors: [],
//               property: null,
//               condition: null,
//               animation: []
//             };


//     func.apply(null, args);
//   };
// }

function loop(on)
{
  "use strict";
  return { action: "loop",
            value: on};
}

function endAction(action)
{
  "use strict";
  return { action: "endAction",
            value: action};
}

function disconnectAction(action)
{
  "use strict";
  return { action: "disconnectAction",
            value: action};
}


function _makeCallback(args) {
  "use strict";
  return function(){
    var a;
    var actors = _actorNameList( args[0] );
    // args[1] is signal name
    for(var i = 2; i < args.length; i++) {
      var d = args[i];
      if(d.action === "call") {
        d.func.apply(null, d.arguments);
      } else if(d.action === "loop") {
        a = _animation(a);
        a.setLooping( d.value );
      } else if(d.action === "endAction") {
        a = _animation(a);
        a.setEndAction( d.value );
      } else if(d.action === "disconnectAction") {
        a = _animation(a);
        a.setDisconnectAction( d.value );
      } else if(d.action === "set") {
        if( d.actors.length ) {
          actors = d.actors;
        }
        for(var actori = 0; actori < actors.length; actori++) {
          var actorName = actors[actori];
          var root = dali.stage.getRootLayer();
          var actor = root.findChildByName( actorName );
          a = _animation(a);
          for(var animi = 0; animi < d.animation.length; animi++) {
            var anim = d.animation[animi];
            var value = anim.value;
            if(typeof value === "function") { // can be a function
              value = value();
            }
            if(anim.animate === "animateTo") {
              a.animateTo(actor, d.property, value, anim.alpha, anim.delay, anim.duration);
            } else if(anim.animate === "animateBy") {
              a.animateBy(actor, d.property, value, anim.alpha, anim.delay, anim.duration);
            } else if(anim.animate === "animatePath") {
              var pathData = anim.path;
              var daliPath;
              if(typeof pathData === "function") {
                pathData = pathData();
              }
              if(typeof pathData === "string") {
                if(pathData in _whenBuilder.paths) {
                  daliPath = _whenBuilder.paths[pathData];
                } else {
                  throw "Path not known";
                }
              } else {
                daliPath = new dali.Path();
                daliPath.points = pathData;
                dali.generateControlPoints(daliPath, 0.35); // @todo remove magic number?
              }
              _animation(a).animatePath(actor, daliPath, anim.forward, anim.alpha, anim.delay, anim.duration);
            } else {
              throw "";
            }
          } // for anims
        } // for actors
      }
    }// for args

    // animation support?
    // else if( d.action === "animate" ) {
    // }

    // actions support??
    // else if( d.action === "then" ) {
    //   if( d.actors.length ) {
    //     actors = d.actors;
    //   }

    if(a) {
      a.play();
    }
  }; // closure function
} // _makeCallback


// doNow(<defaultActor>, ...)
//
// doNow("myimage",
//      call(myfunction),
//      call(myfunction, arg1),
//      and("animation-ends"),
//      set("myimage", "size", to([10,10,10]),
//                             to([10,10,10], 0,3, "ease_in")),
//                             path(0,3, "ease_in", "path0")),
//                             between([0,0,0], [10,10,10], 0,3, "ease_in")),
//                             to([0,0,0], 0, 3)
//     set(tagged("scrollitem), path(0,3, "ease_in", "path0")),
//     set(excludeFrom(tagged("scrollitem"), "myimage"), path(0,3, "ease_in", "path0")),
//     endAction("bake"),
//     discardAction("bake"),
//     loop(true),
//     then("myimage", "hide"),
//     then("quit"),
//     then("play", "myanim"),
//     thenOnChild("myimage", "child", "hide"),
//     animate("name"),
//     animateTo("position", 
//        )
function doNow(args) {
  "use strict";
  var actor = args.shift();
  args.unshift("");     // signal name
  args.unshift(actor);
  var cb = _makeCallback.apply(null, args);
  cb();
}

function _makeSensibleTouched(func, state) { // dali giving all mouse events on touched signal --- is this correct?? @todo
  "use strict";
  return function(actor, touchEvent){
    var doit = false;
    for(var i = 0; i < touchEvent.points.length; i++) {
      if(touchEvent.points[i].state === state) {
        doit = true;
        break;
      }
    }
    if(doit) {
      func.apply( null, arguments);
    }
  };
}

// function _connectToSignal(actors, signalName, func) {
//   "use strict";
//   var root = dali.stage.getRootLayer();
//   var f = func;

//   if( signalName === "touchedDown" ) { // fix as touched signal is really "mouseState"?
//     f = _makeSensibleTouchedDown(func);
//     signalName = "touched";
//   }

//   for(var ai = 0; ai < actors.length; ai++) {
//     var actor = root.findChildByName( actors[ai] );
//     actor.connect( signalName, f );
//   }
// }


// when("myimage", condition("touched", "inside", 0, 100),
// or
// when("myimage", "touched",
//      call(myfunction),
//      call(myfunction, arg1),
//      and("animation-ends"),
//      set("myimage", "size", to([10,10,10]),
//                             to([10,10,10], 0,3, "ease_in")),
//                             path(0,3, "ease_in", "path0")),
//                             between([0,0,0], [10,10,10], 0,3, "ease_in")),
//                             to([0,0,0], 0, 3)
//     set(tagged("scrollitem), path(0,3, "ease_in", "path0")),
//     set(excludeFrom(tagged("scrollitem"), "myimage"), path(0,3, "ease_in", "path0")),
//     endAction("bake"),
//     discardAction("bake"),
//     loop(true),
//     then("myimage", "hide"),
//     then("quit"),
//     then("play", "myanim"),
//     thenOnChild("myimage", "child", "hide"),
//     animate("name"),
//        )
function when() {
  "use strict";
  var actors, property, index;
  var signal = null;
  var condition = null;

  // arg0 can be an actor lookup function; returning an actor array
  actors = _actorNameList(arguments[0]);

  if(typeof arguments[1] === "string") {
    // if string then its a signal
    signal = arguments[1];
  } else {
    // if object then its a condition
    condition = arguments[1];
    // actors can be null which means use the actors in when() call
    if(condition.actors === null) {
      condition.actor = actors[0];
    }
  }

  var root;
  var f;
  var ai;
  var actor;

  if(signal) {
    // _connectToSignal( actors, signal, _makeCallback(arguments) );
    root = dali.stage.getRootLayer();
    f = _makeCallback(arguments);

    if( signal === "touchedDown" ) { // fix as touched signal is really "mouseState"?
      f = _makeSensibleTouched(f, "Down");
      signal = "touched";
    }
    if( signal === "touchedUp" ) { // fix as touched signal is really "mouseState"?
      f = _makeSensibleTouched(f, "Up");
      signal = "touched";
    }
    if( signal === "touchedMotion" ) { // fix as touched signal is really "mouseState"?
      f = _makeSensibleTouched(f, "Motion");
      signal = "touched";
    }
    if( signal === "touchedLeave" ) { // fix as touched signal is really "mouseState"?
      f = _makeSensibleTouched(f, "Leave");
      signal = "touched";
    }
    if( signal === "touchedStationary" ) { // fix as touched signal is really "mouseState"?
      f = _makeSensibleTouched(f, "Stationary");
      signal = "touched";
    }
    if( signal === "touchedInterrupted" ) { // fix as touched signal is really "mouseState"?
      f = _makeSensibleTouched(f, "Interrupted");
      signal = "touched";
    }
    if( signal === "touchedLast" ) { // fix as touched signal is really "mouseState"?
      f = _makeSensibleTouched(f, "Last");
      signal = "touched";
    }

    for(ai = 0; ai < actors.length; ai++) {
      actor = root.findChildByName( actors[ai] );
      actor.connect( signal, f );
    }
  }

  if(condition) {
    // _connectToCondition( actors, condition, _makeCallback(arguments) );
    root = dali.stage.getRootLayer();
    f = _makeCallback(arguments);

    for(ai = 0; ai < actors.length; ai++) {
      actor = root.findChildByName( actors[ai] );
      actor.setPropertyNotification(condition.property,
                                    condition.type, condition.arg0, condition.arg1,
                                    f);
    }
  }

// when("myimage", condition("touched", "inside", 0, 100),
// or
// when("myimage", "touched",
//       ***
//       dali.ActorWrapper.prototype.connect = function(signalName, callback) {
//     if(d.action === "set") {
//       var animationActors = d.actors;
//       if( animationActors.length === 0 ) {
//         animationActors = actors;
//       }
//       var a = new dali.Animation(0);
// // dali.AnimationWrapper.prototype.animateTo = function(object, propertyName, value, alphaFunction, delay, duration) {
// //dali.AnimationWrapper.prototype.animateBy = function(object, propertyName, value, alphaFunction, delay, duration) {
// //dali.AnimationWrapper.prototype.animatePath = function(object, pathObject, forward, alphaFunction, delay, duration) {
//       for(var ai =0; ai < animationActors.length; ai++) {
//         for(var animi =0; animi < d.animation; animi++) {
//           var animEntry = d.animation[animi];
//           if( d.animate == "animatePath") {
//             a.animatePath(animationActors[ai], animEntry.path, animEntry.forward, animEntry.alpha,
//                           animEntry.delay, animEntry.duration);

//           } else if( d.animate == "animateBetween") {
//             a.animateBetween(animationActors[ai], d.property,
//                              animEntry.fromValue, animEntry.toValue,
//                              animEntry.alpha, animEntry.delay, animEntry.duration);
//           } else {
//             // call animateTo,animateBy,
//             a[animEntry.animate](animationActors[ai], d.property, animEntry.value, animEntry.alpha,
//                                  animEntry.delay, animEntry.duration);
//           }
//         }
//   }

  // var animiations = [];

  // for(var i = 2; i < arguments.length; i++) {
  //   var d = arguments[i];
  //   if( d.action === "set" ) {
  //     // var ret = { action: "set",
  //     //             actors: [],
  //     //             property: null,
  //     //             condition: null,
  //     //             animation: []
  //     //           };
  //     for(var ai = 0; ai < d.actors.length; ai++) {
  //       d.actors[ai]
  //     }
  //   }

  //   for(var j = 0; j < d.length; j++) {
  //     if(signal) { // then its signal actions
  //       d.name = signal;
  //       templates[actorName].signals.push(d[j]);
  //     } else { // its property notifications
  //       d.condition = condition;
  //       d.property = property;
  //       templates[actorName].notifications.push(d[j]);

  // index = 2;

  // // but we could also be specifying a property notification so...
  // //
  // var condition;
  // var value;
  // if(typeof arguments[index] === "string") {
  //   var lowerCase = arguments[index].toLowerCase();
  //   if("insideoutsidegreaterthanlessthan".indexOf(lowerCase)) {
  //     property = signal;
  //     signal = null;
  //     condition = lowerCase;
  //     index += 1;
  //     value = arguments[index];
  //     index += 1;
  //     if( condition === "inside" || condition === "outside" ) {
  //       assert( value.length === 2, "Inside/Outside condition must specify min,max");
  //       assert(typeof value[0] === "number" && typeof value[1] === "number", "Conditions must be numbers");
  //     }
  //     if(condition === "lessthan" || condition === "greaterthan") {
  //       assert(value.length === 1, "LessThan/GreaterThan condition must specify one value");
  //       assert(typeof value === "number", "Conditions must be numbers");
  //     }
  //   }
  // }

  // // The reset are objects created by the set or call function
  // var templates = _buildDescription.templates;

  // for( var actorName in actors ) {
  //   // the other args come from functions that generate json
  //   for(var i = index; i < arguments.length; i++) {
  //     var d = arguments[i];
  //     for(var j = 0; j < d.length; j++) {
  //       if(signal) { // then its signal actions
  //         d.name = signal;
  //         templates[actorName].signals.push(d[j]);
  //       } else { // its property notifications
  //         d.condition = condition;
  //         d.property = property;
  //         templates[actorName].notifications.push(d[j]);
  //       }
  //     }
  //   }
  // }

} // when()


// function animation() {
//   "use strict";

//   var name = arguments[0];
//   var anim = {};
//   for(var i = 1; i < arguments.length; i++) {
//     var d = arguments[i];

//     if(d.length === undefined) {  // then array from set

//     } else {
//       extend( anim, d );
//     }
//   }

// }


//
// pseudo dali DSL
//

// stage(
//  layer({
//    width: 400,
//    tag: "scroll"
//    actors:[
//            image({filename:"animage,
//                   name: "myimage",
//                   tag: "scrollitem",
//                   width: 200,
//                   height: 100}),
//            text({text:"sometext,
//                  tag: "scrollitem",
//                  width:10})
//           ]
//  )
// )

// when("myimage", condition("touched", "inside", 0, 100),
// or
// when("myimage", "touched",
//      call(myfunction),
//      call(myfunction, arg1),
//      and("animation-ends"),
//      set("myimage", "size", to([10,10,10]),
//                             to([10,10,10], 0,3, "ease_in")),
//                             path(0,3, "ease_in", "path0")),
//                             between([0,0,0], [10,10,10], 0,3, "ease_in")),
//                             to([0,0,0], 0, 3)
//     set(tagged("scrollitem), path(0,3, "ease_in", "path0")),
//     set(excludeFrom(tagged("scrollitem"), "myimage"), path(0,3, "ease_in", "path0")),
//     then("myimage", "hide"),
//     then("quit"),
//     then("play", "myanim"),
//     thenOnChild("myimage", "child", "hide"),
//     animate("name"),
//        )

// constraints?
// on("myimage", "position", between(0, 100),



//------------------------------------------------------------------------------
// test helper functions
//------------------------------------------------------------------------------

function clear() {
  var root = dali.stage.rootRotationActor;
  var children = root.getChildren();

  for (var i = 0, len = children.length; i < len; i++) {
    root.remove(children[i]);
    children[i].delete(); // delete the wrapper
  }
  //  root.delete(); // wrapper
}

function square(color, size) {
  var a = dali.createSolidColorActor(color, 0, [0, 0, 0, 1], 0);
  a.size = size;
  return a;
}

function threeSquares() {
  var root = dali.stage.rootRotationActor;

  var a = square([1, 0, 0, 1], [200, 200, 0]);
  a.name = "red";
  a.position = [-100, 0, -20];
  root.add(a);
  a.delete();

  a = square([0, 1, 0, 1], [200, 200, 0]);
  a.name = "green";
  a.position = [0, -100, -10];
  root.add(a);
  a.delete();

  a = square([0, 0, 1, 1], [200, 200, 0]);
  a.name = "blue";
  a.position = [0, -100, 0];
  root.add(a);
  a.delete();

  //  root.delete();
}

function threeSquares2() {
  var root = dali.stage.rootRotationActor;

  var red = square([1, 0, 0, 1], [200, 200, 0]);
  red.name = "red";
  red.position = [-100, 0, 20];
  red.size = [10, 10, 0];
  root.add(red);

  var green = square([0, 1, 0, 1], [200, 200, 0]);
  green.name = "green";
  green.position = [0, -100, 0];
  green.orientation = [0, 0, 20];
  green.size = [200, 200, 0];

  var blue = square([0, 0, 1, 1], [200, 200, 0]);
  blue.name = "blue";
  blue.position = [0, 0, 10];
  blue.parentOrigin = [0, 0, 0];
  blue.size = [100, 100, 0];
  green.add(blue);
  root.add(green);

  red.delete(); // wrapper
  green.delete(); // wrapper
  blue.delete(); // wrapper

  //  root.delete();
}

function collectByName(collection) {
  var root = dali.stage.rootRotationActor;
  if (collection === undefined) {
    collection = {};
  }
  var op = function(actor) {
    if (actor.name) {
      collection[actor.name] = actor;
    }
    return true;
  };

  dali.debug.depthVisit(root, op, true);

  return collection;
}


function first() {
  return dali.stage.getRenderTaskList().getTask(0);
}

function second() {
  return dali.stage.getRenderTaskList().getTask(1);
}

function third() {
  return dali.stage.getRenderTaskList().getTask(2);
}

function firstCamera() {
  return dali.stage.getRenderTaskList().getTask(0).getCameraActor();
}

function secondCamera() {
  return dali.stage.getRenderTaskList().getTask(1).getCameraActor();
}

function thirdCamera() {
  return dali.stage.getRenderTaskList().getTask(2).getCameraActor();
}

function resize(w, h) {
  dali.setCanvasSize(w, h);
}

function compareProperties(a1, a2) {
  var props1 = a1.getProperties();
  var props2 = a2.getProperties();
  var props = new Set();
  for (var i = 0, len = props1.size(); i < len; i++) {
    props.add(props1.get(i));
  }

  for (i = 0, len = props2.size(); i < len; i++) {
    var n = props2.get(i);
    if (!props.has(n)) {
      console.log("A1 missing :" + n);
    }
  }

  var doit = function(item) {
    var v1 = a1[item]; // a1.getProperty(item);
    var v2 = a2[item]; // a2.getProperty(item);

    var isSame;

    if (Array.isArray(v1)) {
      isSame = (v1.length === v2.length) && v1.every(function(element, index) {
        return element === v2[index];
      });
    } else {
      isSame = v1 === v2;
    }

    if (!isSame) {
      console.log(item + ": A1= " + v1 + " A2= " + v2);
    }
  };

  props.forEach(doit);
}

var EPSILON = 0.005;

function compareArrays(a, b) {
  "use strict";
  if (Array.isArray(a) && Array.isArray(b)) {
    if (a.length === b.length) {
      for (var i = 0, len = a.length; i < len; i++) {
        if (Array.isArray(a[i])) {
          if (Array.isArray(b[i])) {
            if (!compareArrays(a[i], b[i])) {
              return false;
            }
          } else {
            return false;
          }
        } else {
          if (typeof (a[i]) === "number") {
            if (typeof (b[i]) !== "number") {
              return false;
            } else {
              if (Math.abs(a[i]) > Math.abs(b[i]) + EPSILON ||
                  Math.abs(a[i]) < Math.abs(b[i]) - EPSILON) {
                return false;
              }
            }
          } else {
            if (a[i] !== b[i]) {
              return false;
            }
          }
        }
      }
      return true;
    }
  }
  return false;
}

function countAllProperties() {
  var count = 0;
  var tr = new dali.TypeRegistry();
  var names = tr.getTypeNames();
  for (var i = 0, len = names.size(); i < len; i++) {
    var ti = tr.getTypeInfo(names.get(i));
    var props = ti.getProperties();
    count += props.size();
  }
  return count;
}

function native2ascii(str) {
  "use strict";
  // really this function allows only GLSL permittable chars
  var out = "";
  for (var i = 0; i < str.length; i++) {
    if ( 0x20 <= str.charCodeAt(i) && str.charCodeAt(i) <= 0x7E) {
      // normal characters
      out += str.charAt(i);
    } else if( 0x9 <= str.charCodeAt(i) && str.charCodeAt(i) <= 0xD) {
      // new lines and tabs
      out += str.charAt(i);
    }
  }
  return out;
}


var getGL = function() {
  return canvas.getContext("webgl");
};

var getAnimation = function() {
  "use strict";
  return animationList[animationSelectionIndex].animation;
};

var getActor = function() {
  "use strict";
  return eventHandler.touchedActor;
};


//------------------------------------------------------------------------------
// Unit test class
//------------------------------------------------------------------------------
function Test() {
  "use strict";
}

Test.prototype.hierarchy = function() {
  "use strict";
  console.log("test_hierarchy...");

  // function onTouched(actor) {
  //   // console.log("touched " + actor.$$.ptr + " " + actor.position);
  //   eventHandler.onTouched(actor);
  // }

  var actor = new dali.Actor();
  actor.parentOrigin = [0.5, 0.5, 0.5];
  actor.anchorPoint = [0.5, 0.5, 0.5];
  actor.text = "actor";
  actor.name = actor.text;
  actor.size = [100, 100, 1];
  actor.position = [0, 0, 10];
  dali.stage.add(actor);

  var hello = new dali.Actor();
  // hello.connect("touched", onTouched);
  hello.text = "hello";
  hello.name = hello.text;
  actor.add(hello);

  var hellochild = new dali.Actor();
  // hellochild.connect("touched", onTouched);
  hellochild.text = "hello-child";
  hellochild.name = hellochild.text;
  hello.add(hellochild);

  var hellochild2 = new dali.Actor();
  // hellochild2.connect("touched", onTouched);
  hellochild2.text = "hello-child2";
  hellochild2.name = hellochild2.text;
  hello.add(hellochild2);

  var hellochildchild = new dali.Actor();
  // hellochildchild.connect("touched", onTouched);
  hellochildchild.text = "hello-child-child1";
  hellochildchild.name = "hello-child-child1";
  hellochildchild.name = hellochildchild.text;
  hellochild.add(hellochildchild);


  var depthfirst = actor.findAllChildren();

  assert(actor.getChildCount() === 1);
  // assert(actor.getChildAt(0).text === "hello");
  // assert(actor.findChildByName("hello-child-child1").text = "hello-child-child1");
  // assert(hello.getParent().text === "actor");
  // assert(depthfirst[depthfirst.length - 1].text === "hello-child2");

  var directChildren = actor.directChildren();

  assert(directChildren.length === 1);
  assert(directChildren[0].getId() === hello.getId());

  actor.position = [100, 100, 0];

  var root = dali.stage.rootRotationActor;

  actor.remove(hello);
  assert(actor.getChildCount() === 0);

  actor.add(hello);
  assert(actor.getChildCount() === 1);

  var rootLayerCount = root.getChildCount();
  dali.stage.remove(actor); // check these don't assert
  assert(root.getChildCount() === rootLayerCount - 1);

  dali.stage.add(actor);
  assert(root.getChildCount() === rootLayerCount);

  assert(root.findChildByName("none") === null);

  // actor.connect("touched", onTouched);

  // var inserted = new dali.TextActor(); // TextActor no more RIP
  // inserted.parentOrigin = [0.5, 0.5, 0.5];
  // inserted.anchorPoint = [0.5, 0.5, 0.5];
  // inserted.text = "inserted";
  // inserted.name = inserted.text;
  // inserted.size = [100, 100, 1];
  // inserted.position = [0, 0, 50];
  // actor.insert(1, inserted);
  // assert(actor.getChildAt(1).text === "inserted");

  clear();
  console.log("  -> ok test_hierarchy");
};

Test.prototype.registerProperty = function() {
  "use strict";
  console.log("test_registerPropery...");
  var s = dali.stage;
  var root = s.rootRotationActor;

  var another = new dali.Actor();
  another.parentOrigin = [0.5, 0.5, 0.5];
  another.anchorPoint = [0.5, 0.5, 0.5];
  another.text = "peppa";
  another.name = another.text;
  another.size = [100, 100, 1];
  another.position = [-50, 100, 0];
  root.add(another);

  var c = root.getChildAt(root.getChildCount() - 1);
  //var n = c.getChildCount();
  var p = c.getParent();
  assert(p.getId() == root.getId());

  var matrix = c.worldMatrix;

  assert(matrix.length === 16);

  // todo - no longer supported (?)
  // another.registerProperty("data_output", true);
  // assert(another.getPropertyTypeFromName("data_output") === "BOOLEAN");
  // assert(another.data_output === true);
  // another.data_output = false;
  // assert(another.data_output === false);
  // dali.__updateOnce();
  // another.data_output = 2.5;
  // assert(another.data_output === 2.5);
  // assert(another.getPropertyTypeFromName("data_output") === "FLOAT");

  clear();
  console.log("  -> ok test_registerPropery");
};

Test.prototype.js_math = function() {
  console.log("test_js_math...");
  assert(dali.vectorLength([1, 2, 3, 4]) === Math.sqrt(1 * 1 + 2 * 2 + 3 * 3));
  assert(dali.vectorLengthSquared(dali.normalize([0, 0, 0, 1])) === 0);

  // for(var f=0; f < 6; f+=1)
  // {
  var f = 2;
  assert(1 === dali.vectorLengthSquared(dali.normalize([Math.cos(f) * 10.0,
                                                        Math.cos(f + 1.0) * 10.0,
                                                        Math.cos(f + 2.0) * 10.0,
                                                        1.0
                                                       ])));

  function assertArray(a, b, epsilon) {
    assert(a.length === b.length);
    for (var i = 0, len = a.length; i < len; ++i) {
      assert(a[i] > b[i] - epsilon && a[i] < b[i] + epsilon);
    }
  }

  assertArray(dali.axisAngleToQuaternion([1.0, 2.0, 3.0, Math.PI / 3.0, Math.PI / 2.0]), [0.189, 0.378, 0.567, 0.707], 0.001);

  assertArray(dali.quaternionToAxisAngle([1.1, 3.4, 2.7, 0.932]), [3.03, 9.38, 7.45, 0.74],
              0.01);

  assertArray(dali.vectorCross([0, 1, 0], [0, 0, 1]), [1, 0, 0], 0.001);

  assertArray(dali.vectorAdd([1, 2, 3], [2, 3, 4], [1, 1, 1]), [4, 6, 8], 0.001);

  var mq = dali.vectorAdd(dali.vectorCross([0.045, 0.443, 0.432], [0.612, 0.344, -0.144]),
                          dali.vectorByScalar([0.612, 0.344, -0.144], 0.784),
                          dali.vectorByScalar([0.045, 0.443, 0.432], 0.697));

  assertArray(dali.quatByQuat([0.045, 0.443, 0.432, 0.784], [0.612, 0.344, -0.144, 0.697]), [mq[0], mq[1], mq[2], (0.784 * 0.697) - dali.vectorDot([0.045, 0.443, 0.432], [0.612, 0.344, -0.144])],
              0.001);

  clear();
  console.log("  -> ok test_js_math");
};

Test.prototype.getset = function() {
  "use strict";
  console.log("test_getset...");
  threeSquares();
  var col = {};
  collectByName(col);
  var actor = col.red;
  var p = actor.position;
  actor.position = [1, 1, 1];
  assert(compareArrays(actor.position, [1, 1, 1]));
  actor.position = [3, 3, 3];
  assert(compareArrays(actor.position, [3, 3, 3]));
  actor.position = p;

  clear();
  console.log("  -> ok test_getset");
};

Test.prototype.animation_spline = function() {
  "use strict";
  console.log("test_animation_spline...");
  threeSquares();
  var col = {};
  collectByName(col);
  var actor = col.red;

  var a = new dali.Animation(0);
  var path = new dali.Path();

  path.points = [
    [-150, -50, 0],
    [0.0, 70.0, 0.0],
    [190.0, -150.0, 0.0]
  ];

  assert(compareArrays(path.points, [
    [-150, -50, 0],
    [0.0, 70.0, 0.0],
    [190.0, -150.0, 0.0]
  ]));

  dali.generateControlPoints(path, 0.35);

  assert(compareArrays(path.controlPoints, [
    [-97.5, -8, 0],
    [-66.94940948486328, 76.16658020019531, 0],
    [101.31224060058594, 60.66832733154297, 0],
    [123.5, -73, 0]
  ]));

  a.setDuration(3);
  a.animatePath(actor, path, [1, 0, 0], dali.AlphaFunction.LINEAR, 0, 3);
  a.play();

  function checkPos() {
    assert(actor.position = path.points[2]);
    clear();
    actor.delete();
    path.delete();
    a.delete();
    console.log("  -> ok test_animation_spline");
  }

  window.setTimeout(checkPos, 4000);
};

Test.prototype.animation = function() {
  "use strict";
  console.log("test_animation...");
  threeSquares();
  var col = {};
  collectByName(col);
  var actor = col.red;

  var a = new dali.Animation(0);
  a.setDuration(3);
  a.animateTo(actor, "position", [20, 0, 0], dali.AlphaFunction.LINEAR, 0, 3);
  a.play();

  function checkAnimateBetween() {
    assert(actor.position = [0, 0, -30]);
    clear();
    a.delete();
    actor.delete();

    console.log("  -> ok test_animation");
  }

  function checkAnimateBy() {
    assert(actor.position = [120, 100, 0]);

    a.clear();
    a.animateBetween(actor,
                     "position", [ [ 0,  [10,20,30] ],
                                   [ 1.0,[0, 0, -30] ] ],
                     "linear",
                     0,
                     3,
                     "linear");
    a.play();
    window.setTimeout(checkAnimateBetween, 4000);
  }

  function checkAnimateTo() {
    assert(actor.position = [20, 0, 0]);
    actor.position = [100, 100, 0];

    a.clear(); // var a = new dali.Animation(0);
    a.setDuration(3);
    a.animateBy(actor, "position", [20, 0, 0], dali.AlphaFunction.LINEAR, 0, 3);
    a.play();

    window.setTimeout(checkAnimateBy, 4000);
  }

  window.setTimeout(checkAnimateTo, 4000);
};

Test.prototype.onePane = function() {
  var w = dali.canvas.width;
  var h = dali.canvas.height;
  var col = dali.getClearColor(0);
  dali.onePane();
  dali.setFrontView(0, 0, 0, w, h);
  dali.setClearColor(0, col);
};

Test.prototype.threePane = function() {
  var w = dali.canvas.width;
  var h = dali.canvas.height;
  dali.threePane();
  dali.setClearColor(0, [0.4, 0, 0, 1]);
  dali.setClearColor(1, [0, 0.4, 0, 1]);
  dali.setClearColor(2, [0, 0, 0.4, 1]);
  dali.setFrontView(0, 0, 0, w / 2 - 5, h);
  dali.setTopView(1, w / 2, 0, w / 2, h / 2 - 5);
  dali.setRightView(2, w / 2, h / 2 + 5, w / 2, h / 2 - 5);
};

Test.prototype.twoPane = function() {
  var w = dali.canvas.width;
  var h = dali.canvas.height;
  dali.twoPane();
  dali.setFrontView(0, 0, 0, w / 2 - 10, h);
  dali.setTopView(1, 210, 0, w / 2 - 10, h);
  dali.setClearColor(0, [0.4, 0, 0, 1]);
  dali.setClearColor(1, [0, 0.4, 0, 1]);
};

Test.prototype.views = function() {
  "use strict";
  console.log("test_views");

  var w = dali.canvas.width;
  var h = dali.canvas.height;
  var col = dali.getClearColor(0);
  console.log(col);

  function one() {
    dali.onePane();
    dali.setFrontView(0, 0, 0, w, h);
    dali.setClearColor(0, col);
  }

  function three() {
    dali.threePane();
    dali.setClearColor(0, [0.4, 0, 0, 1]);
    dali.setClearColor(1, [0, 0.4, 0, 1]);
    dali.setClearColor(2, [0, 0, 0.4, 1]);
    dali.setFrontView(0, 0, 0, w / 2 - 5, h);
    dali.setTopView(1, w / 2, 0, w / 2, h / 2 - 5);
    dali.setRightView(2, w / 2, h / 2 + 5, w / 2, h / 2 - 5);

    window.setTimeout(one, 1000);
  }

  function two() {
    dali.twoPane();
    dali.setFrontView(0, 0, 0, w / 2 - 10, h);
    dali.setTopView(1, 210, 0, w / 2 - 10, h);
    dali.setClearColor(0, [0.4, 0, 0, 1]);
    dali.setClearColor(1, [0, 0.4, 0, 1]);

    window.setTimeout(three, 1000);
  }

  one();

  window.setTimeout(two, 1000);
};

Test.prototype.blinking = function() {
  "use strict";
  var layer = new dali.Layer();
  layer.name = "frameLayer";
  dali.stage.add(layer);

  var a = dali.createSolidColorActor([0.5, 0.5, 0.5, 1],
                                     false, [0, 0, 0, 1],
                                     0);
  a.size = [100,100,1];

  layer.add(a);

  var camera = firstCamera();

  camera.position = [ camera.position[0]+10, camera.position[1]+20, camera.position[2] + 10 ];

  layer.delete();   // wrapper
  a.delete();       // wrapper
  camera.delete();  // wrapper
};

Test.prototype.uniformMetaData = function() {
  for(var i = 0; i < shaderSources.length; i++) {
    console.log(dali.uniformMetaData(shaderSources[i].vertex, shaderSources[i].fragment));
  }
};

Test.prototype.signals = function() {
  "use strict";
  console.log("test_signals...");

  function onStage() {
    console.log("   -> ok test signals");
    //eventHandler.onTouched(actor);
  }

  var actor = new dali.Actor();
  actor.parentOrigin = [0.5, 0.5, 0.5];
  actor.anchorPoint = [0.5, 0.5, 0.5];
  actor.text = "actor";
  actor.name = actor.text;
  actor.size = [100, 100, 1];
  actor.position = [0, 0, 10];

  actor.connect("on-stage", onStage);

  dali.stage.add(actor);
};

Test.prototype.shaderInfo = function() {
  "use strict";
  var info = new dali.ShaderInfo();

  var vertex;
  var fragment;

  vertex = "\n" +
    "attribute mediump vec3 aPosition;\n" +
    "attribute mediump vec2 aTexCoord;\n" +
    "varying mediump vec2 vTexCoord;\n" +
    "uniform mediump vec3 uSize;\n" +
    "// uniform mediump vec3 unusedUniform;\n" +
    "uniform mediump mat4 uModelView;\n" +
    "uniform mediump mat4 uProjection;\n" +
    "\n" +
    "void main(void)\n" +
    "{\n" +
    "  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n" +
    "  gl_Position.xyz *= uSize;\n" +
    "  vTexCoord = aTexCoord;\n" +
    "}\n";

  fragment = "precision mediump float;\n" +
    "\n" +
    "uniform sampler2D sTexture;\n" +
    "uniform mediump vec4 uMyColor; // {min:[0,0,0,0], max:[1,1,1,1]}\n" +
    "uniform mediump vec4 uColor;\n" +
    "varying mediump vec2 vTexCoord;\n" +
    "\n" +
    "void main()\n" +
    "{\n" +
    "  gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor * uMyColor;\n" +
    "}\n";

  var canvas = document.createElement("canvas");
  var meta = info.fromCompilation(canvas.getContext("webgl"), vertex, fragment);

  var uniforms = { uSize: 1,
                   uModelView: 1,
                   uProjection: 1,
                   uMyColor: 1,
                   uColor: 1
                 };

  assert(meta.hasError === false);
  var name;
  var metaUniformName;
  var found;

  for(name in uniforms) {
    found = false;
    for(metaUniformName in meta.uniforms) {
      if(metaUniformName === name) {
        found = true;
        break;
      }
    }
    assert(found, "missing:" + name);
  }

  assert(compareArrays(meta.uniformUISpec.uMyColor.min, [0, 0, 0, 0]));
  assert(compareArrays(meta.uniformUISpec.uMyColor.max, [1, 1, 1, 1]));


  meta = info.fromRegEx(vertex, fragment);

  assert(meta.hasError === false);

  for(name in uniforms) {
    found = false;
    for(metaUniformName in meta.uniforms) {
      if(metaUniformName === name) {
        found = true;
        break;
      }
    }
    assert(found, "missing:" + name);
  }

  assert(compareArrays(meta.uniformUISpec.uMyColor.min, [0, 0, 0, 0]));
  assert(compareArrays(meta.uniformUISpec.uMyColor.max, [1, 1, 1, 1]));

  console.log("   -> ok test shaderInfo");
};

//------------------------------------------------------------------------------
// regression test
//------------------------------------------------------------------------------

Test.prototype.regression = function() {
  "use strict";
  this.hierarchy();
  this.registerProperty();
  this.js_math();
  this.getset();
  this.animation();
  this.animation_spline();
  // this.shadereffect1();
  this.views();
  this.signals();
  this.shaderInfo();
};

Test.prototype.remote_execution = function() {
  "use strict";
  this.regression();
};

Test.prototype.stacking = function() {
  "use strict";

  eventHandler.selectActor( dali.stage.rootRotationActor );

  var a = dali.createSolidColorActor([1.0, 0.0, 0.0, 1.0],
                                     false, [0, 0, 0, 1],
                                     0);
  app.addActor(a);

  eventHandler.selectActor( a );

  var b = dali.createSolidColorActor([0.0, 0.0, 1.0, 1.0],
                                     false, [0, 0, 0, 1],
                                     0);
  app.addActor(b);

  a.sizeWidth = a.sizeHeight = 100;
  b.sizeWidth = b.sizeHeight = 50;

  b.sizeWidth = 150;

  return [a, b];
};


var runTest = function(functionName) {
  "use strict";

  clear();

  dali.stage.setBackgroundColor([0.3, 0.3, 0.3, 1]);

  var test = dali[functionName]();

  if( test )
  {
    if( !test.complete() )
    {
      function check() {
        if( !test.complete() ) {
          window.setTimeout(checkPos, 500);
        } else {
          console.log("test success");
        }
      }
    }
  }

};

//------------------------------------------------------------------------------
// scratch
//------------------------------------------------------------------------------
function animateShaderEffect2(actor) {
  "use strict";
  var shader = new dali.ShaderEffect({
    vertex: shaderSource2.vertex,
    fragment: shaderSource2.fragment
  });

  actor.setShaderEffect(shader);

  var final = [5, 5, 5, 1];

  var a = new dali.Animation(0);
  a.setDuration(3);
  a.setLooping(true);
  a.animateTo(shader, "weight", final, dali.AlphaFunction.LINEAR, 0, 3);

  a.play();

  a.delete();

}


var testfile = { // output from clara.io "Threejs scene output"
  "metadata": {
    "version": 4.3, // This isnt as its documented on threejs website
    "type": "Object", // and the general format looks more like format V3.
    "generator": "ObjectExporter"
  },
  "geometries": [{
    "uuid": "2f167add-e571-47c2-9da2-6f0e45cc1119",
    "type": "Geometry",
    "data": {
      "vertices": [
        0.5,
        0.5,
        0.5,
        0.5,
        0.5, -0.5,
        0.5, -0.5,
        0.5,
        0.5, -0.5, -0.5, -0.5,
        0.5, -0.5, -0.5,
        0.5,
        0.5, -0.5, -0.5, -0.5, -0.5, -0.5,
        0.5
      ],
      "normals": [
        1,
        0,
        0, -1,
        0,
        0,
        0,
        1,
        0,
        0, -1,
        0,
        0,
        0,
        1,
        0,
        0, -1
      ],
      "uvs": [
        [
          0,
          1,
          0,
          0,
          1,
          0,
          1,
          1
        ]
      ],
      "faces": [
        56,

        0,
        2,
        3,

        0,
        1,
        2,

        0,
        0,
        0,

        0,

        56,
        0,
        3,
        1,
        0,
        2,
        3,
        0,
        0,
        0,
        0,
        56,
        4,
        6,
        7,
        0,
        1,
        2,
        1,
        1,
        1,
        1,
        56,
        4,
        7,
        5,
        0,
        2,
        3,
        1,
        1,
        1,
        1,
        56,
        4,
        5,
        0,
        0,
        1,
        2,
        2,
        2,
        2,
        2,
        56,
        4,
        0,
        1,
        0,
        2,
        3,
        2,
        2,
        2,
        2,
        56,
        7,
        6,
        3,
        0,
        1,
        2,
        3,
        3,
        3,
        3,
        56,
        7,
        3,
        2,
        0,
        2,
        3,
        3,
        3,
        3,
        3,
        56,
        5,
        7,
        2,
        0,
        1,
        2,
        4,
        4,
        4,
        4,
        56,
        5,
        2,
        0,
        0,
        2,
        3,
        4,
        4,
        4,
        4,
        56,
        1,
        3,
        6,
        0,
        1,
        2,
        5,
        5,
        5,
        5,
        56,
        1,
        6,
        4,
        0,
        2,
        3,
        5,
        5,
        5,
        5
      ]
    }
  }],
  "materials": [{
    "uuid": "14D499F1-27EF-45BF-A457-FD24DAB11205",
    "type": "MeshPhongMaterial",
    "color": 11579568,
    "ambient": 11579568,
    "emissive": 0,
    "specular": 0,
    "shininess": 50,
    "opacity": 1,
    "transparent": false,
    "wireframe": false
  }],
  "object": {
    "uuid": "BFEFB48D-0E6E-46A6-8568-5E258BA17078",
    "type": "Scene",
    "matrix": [
      1,
      0,
      0,
      0,
      0,
      1,
      0,
      0,
      0,
      0,
      1,
      0,
      0,
      0,
      0,
      1
    ],
    "children": [{
      "uuid": "aa901bec-9e47-4b3b-bf3c-4efb0fe5d298",
      "name": "Box",
      "type": "Mesh",
      "geometry": "2f167add-e571-47c2-9da2-6f0e45cc1119",
      "material": "14D499F1-27EF-45BF-A457-FD24DAB11205",
      "castShadow": true,
      "receiveShadow": true,
      "matrix": [
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1
      ]
    }]
  }
};



//
//
// Event/widget setup
//
//

//------------------------------------------------------------------------------
//
// Helper functions
//
//------------------------------------------------------------------------------
function rebuildDropdown(dropdownElement, db, dbStoreName, optionalItemDataFunc) {
  "use strict";
  removeAllChildren(dropdownElement);

  var func = optionalItemDataFunc;
  if( !func ) {
    func = function(name) {
      return [null, name];
    };
  }

  var li;
  var a;
  var names = db.getKeys(dbStoreName);

  for(var i = 0; i < names.length; i++) {
    li = document.createElement("li");
    var hrefText = func(names[i]);
    if( hrefText[0] ) {
      a = document.createElement("a");
      a.href = hrefText[0];
      a.text = hrefText[1];
    } else {
      a = document.createElement("p");
      a.textContent = hrefText[1];
    }
    li.appendChild(a);
    dropdownElement.appendChild(li);
  }
}


//------------------------------------------------------------------------------
//
// UI General
//
//------------------------------------------------------------------------------
function UIApp() {
  "use strict";
  var self = this;

  self.modalQuestionYesFunction = null;
  self.modalQuestionNoFunction = null;

  var _modalQuestionYes = function() {
    if(self.modalQuestionYesFunction) {
      self.modalQuestionYesFunction();
    }
    self.modalQuestionYesFunction = null;
    self.modalQuestionNoFunction = null;
  };

  var _modalQuestionNo = function() {
    if(self.modalQuestionNo) {
      self.modalQuestionNo();
    }
    self.modalQuestionYesFunction = null;
    self.modalQuestionNoFunction = null;
  };

  var _modalInputOk = function() {
    if(self.modalInputOkFunction) {
      var input = $("#modalInputText")[0];
      self.modalInputOkFunction(input.value);
    }
    self.modalInputOkFunction = null;
    self.modalInputCancelFunction = null;
  };

  var _modalInputCancel = function() {
    if(self.modalInputCancel) {
      self.modalInputCancel();
    }
    self.modalInputOkFunction = null;
    self.modalInputCancelFunction = null;
  };

  $("#modalQuestionYes")[0].addEventListener("click", _modalQuestionYes);
  $("#modalQuestionNo")[0].addEventListener("click", _modalQuestionNo);

  self.modalInputYesFunction = null;
  self.modalInputNoFunction = null;

  $("#modalInputOk")[0].addEventListener("click", _modalInputOk);
  $("#modalInputCancel")[0].addEventListener("click", _modalInputCancel);

}

UIApp.prototype.messageBoxHTML = function(innerHtmlText) {
  "use strict";
  var modalBody = $("#modalCodeInfoBody")[0];
  removeAllChildren(modalBody);
  modalBody.innerHTML = innerHtmlText;
  $("#modalCodeInfo").modal("show");
};

UIApp.prototype.messageBox = function(message) {
  "use strict";
  this.messageBoxHTML("<p>" + message + "</p>");
};

UIApp.prototype.questionBox = function(message, yesFunction, noFunction) {
  "use strict";
  var modalBody = $("#modalQuestionBody")[0];
  removeAllChildren(modalBody);
  modalBody.innerHTML = "<p>" + message + "</p>";

  this.modalQuestionYesFunction = yesFunction;
  this.modalQuestionNoFunction = noFunction;

  $("#modalQuestion").modal("show");
};

UIApp.prototype.codeInformationBox = function(code) {
  "use strict";
  var modalBody = $("#modalCodeInfoBody")[0];
  removeAllChildren(modalBody);
  modalBody.innerHTML = "<pre>" + code + "</pre>";
  $("#modalCodeInfo").modal("show");
};

UIApp.prototype.inputbox = function(message, okFunction, cancelFunction) {
  "use strict";
  var modalBody = $("#modalInputBody")[0];
  removeAllChildren(modalBody);
  modalBody.innerHTML = "<p>" + message + "</p>";

  this.modalInputOkFunction = okFunction;
  this.modalInputCancelFunction = cancelFunction;

  $("#modalInput").modal("show");
};

UIApp.prototype.getTypedBuffer = function(file, callback) {
  "use strict";
  if (!file) {
    return;
  }
  var reader = new FileReader();
  reader._theFilename = file.name;
  reader.onload = function(// e
  ) {
    var uint8View = new Uint8Array(reader.result); // convert ArrayBuffer into a typed array?
    callback(file.name, uint8View);
  };

  reader.readAsArrayBuffer(file);
};


//------------------------------------------------------------------------------
//
// Javascript UI Tab
//
//------------------------------------------------------------------------------
function UIJavascriptTab() {
  "use strict";
  var self = this;
  self.bufferPrepend = "Buffer Name:";

  this.addNewBuffer = function() {
    var alreadyExists = function() {
      self.addNewBuffer();
    };

    var inputOk = function(name) {
      var data = database.readJavascript(name);
      if(data) {
        uiApp.messageBox("Name already exsists", alreadyExists);
      } else {
        var newData = {name: name, source: ""};
        database.writeJavascript(newData);
        self.rebuildDropdown(database);
      }
    };

    uiApp.inputbox("BufferName?", inputOk);

  };

  this.renameBuffer = function() {
    var writeOK = function() {
      var openRequest = database.open();

      openRequest.onsuccess = function(event) {
        var db = event.target.result;
        self.rebuildDropdown(database);
        db.close();
      };
    };

    var inputOk = function(name) {
      var data = uiJavascriptTab.getData();
      data.name = name;
      database.writeJavascript(data, writeOK);
    };

    uiApp.inputbox("BufferName?", inputOk);

  };

  this.loadJavascript = function(name) {
    var data = database.readJavascript(name);

    // save current
    var currentData = self.getData();
    if(currentData) {
      database.writeJavascript(currentData);
    }

    self.setData(data);
  };

  ace.require("ace/ext/language_tools");
  var editor = ace.edit("editorJavascript");

  var _thisFunctions = [];
  for(var attr in this) {
    if( !(attr.startsWith("_") || attr.startsWith("dynCall") || attr.startsWith("invoke") ) ) {
      if( typeof this[attr] === "function") {
        _thisFunctions.push( attr );
      }
    }
  }

  var myCompleter = {
    getCompletions: function(theEditor, session, pos, prefix, callback) {
      var wordlist = [];
      if(prefix === "dali.") {
        for(attr in dali) {
          if( !(attr.startsWith("_") || attr.startsWith("dynCall") || attr.startsWith("invoke") ) ) {
            if( typeof dali[attr] === "function") {
              wordlist.push( attr );
            }
          }
        }
      } else {
        wordlist = _thisFunctions;
      }
      callback(null, wordlist.map(function(word) {
        return {
          caption: word,
          value: word,
          meta: "static"
        };
      }));
    }
  };

  editor.completers = [ myCompleter ];

  editor.setOptions({
    enableBasicAutocompletion: true,
    enableSnippets: true,
    enableLiveAutocompletion: true
  });

  document.getElementById("btnJavascriptSourceJson").addEventListener(
    "click",
    function(/*e*/) {
      self.showModalJSON();
    });

  document.getElementById("btnJavascriptClearRun").addEventListener(
    "click",
    function(/*e*/) {
      self.clearStageAndEval();
    });

  document.getElementById("btnJavascriptRun").addEventListener(
    "click",
    function(/*e*/) {
      self.eval();
    });

  document.getElementById("btnJavascriptAddBuffer").addEventListener(
    "click",
    function(/*e*/) {
      self.addNewBuffer();
    });

  document.getElementById("btnJavascriptRenameBuffer").addEventListener(
    "click",
    function(/*e*/) {
      self.renameBuffer();
    });

  this.currentName = undefined;

  var names = database.readJavascriptNames();
  if(names.length) {
    self.loadJavascript(names[0]); // load first javascript buffer
  }

}

UIJavascriptTab.prototype.getData = function() {
  "use strict";
  var e = ace.edit("editorJavascript");
  var ascii = native2ascii(e.getSession().getValue());

  if(this.currentName !== undefined) {
    return { name: this.currentName,
             source: ascii };
  } else {
    return undefined;
  }
};

UIJavascriptTab.prototype.setData = function(data) {
  "use strict";
  this.currentName = data.name;
  // set new
  var editor = ace.edit("editorJavascript");
  editor.getSession().setValue(data.source);

  $("#textJavascriptName").html(this.bufferPrepend + data.name);

};

UIJavascriptTab.prototype.showModalJSON = function() {
  "use strict";
  var e = ace.edit("editorJavascript");
  var ascii = native2ascii(e.getSession().getValue());

  var lines = ascii.split("\n");

  var data = "{source:";

  for(var i = 0; i < lines.length; i++) {
    data += "\"" + lines[i].replace(/"/g, "\\\"") + "\\n\" +\n";
  }

  data += "\n\"\"}";

  // data = data.replace(/<([^ ]*)/g, "< $1"); // for loops are interpreted by browser as tags and dont print?

  // data = data.replace(/"/g, "\\\""); // for loops are interpreted by browser as tags and dont print?

  // data = data.replace(/<([^ ]*)/g, "< $1"); // for loops are interpreted by browser as tags and dont print?

  uiApp.codeInformationBox( data );

};

UIJavascriptTab.prototype.clearStageAndEval = function() {
  "use strict";
  var e = ace.edit("editorJavascript");
  var ascii = native2ascii(e.getSession().getValue());
  clear();
  // as of ecma5 an indirect call like this is in global scope
  var globalEval = eval;
  globalEval(ascii);
};

UIJavascriptTab.prototype.eval = function() {
  "use strict";
  var e = ace.edit("editorJavascript");
  var ascii = native2ascii(e.getSession().getValue());
  // as of ecma5 an indirect call like this is in global scope
  var globalEval = eval;
  globalEval(ascii);
};

UIJavascriptTab.prototype.rebuildDropdown = function(db) {
  "use strict";
  rebuildDropdown( document.getElementById("javascriptDropDown"),
                   db,
                   "javascript",
                   function(name) {
                     return ["javascript:uiJavascriptTab.loadJavascript(\"" + name + "\")",
                             name];
                   }
                 );
};


//------------------------------------------------------------------------------
//
// Image UI Tab
//
//------------------------------------------------------------------------------

/**
 * Get or create a dali image from the image buffer.
 *
 * Global function for use in javascript buffers or the console.
 * @param {string} shader buffer name
 * @return  {Object} dali.Image
 */
function imageFromUiBuffer(name) {
  "use strict";
  var img = uiImageTab.getDaliImage(name);
  assert(img, "Could not find image:" + name);
  return img;
}


/**
 * Manages UI image tab.
 *
 * @constructor
 */
function UIImageTab() {
  "use strict";
  var self = this;
  this.imagesCreated = {}; // dali shader objects

  /********** methods **********/
  this.getBufferImageRGB = function(file, callback) {
    if (!file) {
      return;
    }

    var img = new HTML5Image(); // the renamed Image()

    var objectUrl = window.URL.createObjectURL(file);

    img.onload = function( //e
    ) {
      var imageCanvas = document.createElement("canvas");
      imageCanvas.width = img.width; // naturalWidth;
      imageCanvas.height = img.height; // naturalHeight;
      var context = imageCanvas.getContext("2d");
      context.drawImage(img, 0, 0 );
      var imageData = context.getImageData(0, 0, img.naturalWidth, img.naturalHeight); // <-ImageData
      callback(file.name, imageData);
      window.URL.revokeObjectURL(objectUrl);
    };

    img.src = objectUrl;

  };


  /**
   * Add new image data
   *
   * @param {object} ImageData object
   */
  this.addNewBuffer = function(imageData) {
    var alreadyExists = function() {
      self.addNewBuffer(imageData);
    };

    var inputOk = function(name) {
      var oldData = database.readImage(name);
      if(oldData) {
        uiApp.messageBox("Name already exsists", alreadyExists);
      } else {
        database.writeImage(name, imageData);
        self.rebuildDropdown(database);
      }
    };

    uiApp.inputbox("BufferName?", inputOk);
  };

  this.renameBuffer = function() {
    var inputOk = function(name) {
      var data = uiJavascriptTab.getData();
      data.name = name;
      database.writeJavascript(data);
      self.rebuildDropdown(database);
    };

    uiApp.inputbox("BufferName?", inputOk);
  };

  /**
   * Load image into the UI Image tab. (used for dynamically generated html dropdowns)
   *
   * @param {string} name of image buffer
   */
  this.loadImage = function(name) {
    self.setData( database.readImage(name) );
  };

  /********** init **********/

  document.getElementById("btnImageAddBuffer").addEventListener(
    "change",
    function() {
      var fileInput = document.getElementById("btnImageAddBuffer");
      var file = fileInput.files[0];
      self.getBufferImageRGB(
        file,
        function(name, typedBuffer) {
          self.addNewBuffer(typedBuffer);
        });
    });


  document.getElementById("btnImageRenameBuffer").addEventListener(
    "click",
    function(/*e*/) {
      self.renameBuffer();
    });

  var names = database.readImageNames();
  if(names.length) {
    // load first image buffer
    self.setData( database.readImage(names[0]) );
  }

}

/**
 * Get or create a dali image from the image buffer.
 *
 * @return  {Object} dali.Image
 */
UIImageTab.prototype.getDaliImage = function(name) {
  "use strict";
  if(name in this.imagesCreated) {
    return this.imagesCreated[name];
  } else {
    var imageData = database.readImage(name);
    var uint8array = new Uint8Array(imageData.data);
    var image = new dali.BufferImage(uint8array, imageData.width, imageData.height, dali.PixelFormat.RGBA8888);
    this.imagesCreated[name] = image;
    return image;
  }
};

UIImageTab.prototype.rebuildDropdown = function(db) {
  "use strict";
  rebuildDropdown( document.getElementById("imageDropDown"),
                   db,
                   "images",
                   function(name) {
                     return ["javascript:uiImageTab.loadImage(\"" + name + "\")",
                             name];
                   }
                 );
};

UIImageTab.prototype.setData = function(data) {
  "use strict";
  var imageViewCanvas = document.getElementById("imageViewCanvas"); // createElement("canvas");
  imageViewCanvas.width = data.width; // naturalWidth;
  imageViewCanvas.height = data.height; // naturalHeight;
  var context = imageViewCanvas.getContext("2d");
  context.putImageData( data, 0, 0 );

};

//------------------------------------------------------------------------------
//
// Shader UI Tab
//
//------------------------------------------------------------------------------
/**
 * Get or create a dali shader from the shader buffer.
 *
 * Global function for use in javascript buffers or the console.
 * @param {string} shader buffer name
 * @return  {Object} dali.Shader
 */
function shaderFromUiBuffer(name) {
  "use strict";
  var shader = uiShaderTab.getDaliShader(name);
  assert(shader, "Could not find/compile shader:" + name);
  return shader;
}

function shaderInfoFromUiBuffer(name) {
  "use strict";
  return uiShaderTab.getShaderInfo(name);
}

/**
 * Manage shader UI tab events.
 * Holds dali shader objects created from shader buffers.
 *
 * @constructor
 * @return {Object} UIShaderTab
 */
function UIShaderTab() {
  "use strict";
  var self = this;
  this.bufferPrepend = "Buffer Name:";
  this.inhibitCheckAndUpdateShader = false;

  this.shadersCreated = {}; // dali shader objects
  document.getElementById("btnShaderAddBuffer").addEventListener(
    "click",
    function(/*e*/) {
      self.addNewBuffer();
    });

  document.getElementById("btnShaderRenameBuffer").addEventListener(
    "click",
    function(/*e*/) {
      self.renameBuffer();
    });

  document.getElementById("btnShaderSourceJSON").addEventListener(
    "click",
    function(/*e*/) {
      self.showModalJSON();
    });

  document.getElementById("btnShaderSourceC").addEventListener(
    "click",
    function(/*e*/) {
      self.showModalC();
    });

  document.getElementById("btnShaderSourceJS").addEventListener(
    "click",
    function(/*e*/) {
      self.showModalJS();
    });


  this.addNewBuffer = function() {
    var alreadyExists = function() {
      self.addNewBuffer();
    };

    var inputOk = function(name) {
      var data = database.readJavascript(name);
      if(data) {
        uiApp.messageBox("Name already exsists", alreadyExists);
      } else {
        var newData = {name: name, fragment: "", vertex: "", hints: ""};
        database.writeShader(newData);
        self.rebuildDropDown(database);
      }
    };

    uiApp.inputbox("BufferName?", inputOk);

  };

  this.renameBuffer = function() {
    var inputOk = function(name) {
      var data = self.getData();
      data.name = name;
      database.writeShader(data);
      $("#textShaderName").html(self.bufferPrepend + name);
      self.rebuildDropdown(database);
    };

    uiApp.inputbox("BufferName?", inputOk);
  };

  this.loadShader = function(name) {

    var data = database.readShader(name);

    // save current
    var currentData = self.getData();
    if(currentData) {
      database.writeShader(currentData);
    }

    // so that we can change vertex & fragment as an atomic operation
    self.inhibitCheckAndUpdateShader = true;

    // otherwise this will trigger a checkAndUpdateShader() after
    // we've only changed the vertex shader (ie with mismatching fragment shader)
    self.setData(data);

    self.inhibitCheckAndUpdateShader = false;

    self.checkAndUpdateShader();

  };

  /**
   * Set the UI tab data of vertex/fragment strings.
   *
   * @param {Object} shader data object
   */
  this.setData = function(options) {
    var e;
    var vertex = "";
    var fragment = "";
    var hints = "";

    vertex = options.vertex;
    fragment = options.fragment;
    hints = options.hints;

    $("#requiresSelfDepthTest").prop("checked", (hints.search("requiresSelfDepthTest") >= 0));
    $("#outputIsTransparent").prop("checked", (hints.search("outputIsTransparent") >= 0));
    $("#outputIsOpaque").prop("checked", (hints.search("outputIsOpaque") >= 0));
    $("#modifiesGeometry").prop("checked", (hints.search("modifiesGeometry") >= 0));

    // do this after setting up the checkboxes as it will trigger storing the checkbox state
    // in the actorIdToShaderSet map
    e = ace.edit("editorVertex");
    e.getSession().setValue(vertex);

    e = ace.edit("editorFragment");
    e.getSession().setValue(fragment);

    $("#textShaderName").html(self.bufferPrepend + options.name);

  };

  var shaderTab = $("#tab2primary")[0];
  var lastClassName = shaderTab.className;
  window.setInterval( function() {
    var className = shaderTab.className;
    if (className !== lastClassName) {
      //
      // an attempt to get the editboxes to display the correct content.
      // when you setValue() the content and they aren't visible then
      // they dont update properly until you click in the box
      //
      var e = ace.edit("editorVertex");
      e.setShowInvisibles(true);
      e.setShowInvisibles(false);

      e = ace.edit("editorFragment");
      e.setShowInvisibles(true);
      e.setShowInvisibles(false);
      lastClassName = className;
    }
  }, 1);

  var names = database.readShaderNames();
  if(names.length) {
    // load first image buffer
    self.setData( database.readShader(names[0]) );
  }

  var editor = ace.edit("editorVertex");
  editor.getSession().addEventListener("change", function() {
    self.checkAndUpdateShader();
  });

  editor = ace.edit("editorFragment");
  editor.getSession().addEventListener("change", function() {
    self.checkAndUpdateShader();
  });

}

/**
 * Get or create a dali shader from the shader buffer.
 *
 * @return  {Object} dali.Shader
 */
UIShaderTab.prototype.getDaliShader = function(name) {
  "use strict";
  if(name in this.shadersCreated) {
    return this.shadersCreated[name].daliShader;
  } else {
    var ret = null;
    var data = database.readShader(name);

    if(this.isCompilable(data)) {
      var daliShader = new dali.Shader(data.vertex,
                                       data.fragment,
                                       this.getDaliShaderHints(data.hints));

      var shaderInfo = new dali.ShaderInfo();
      var info = shaderInfo.fromCompilation( canvas.getContext("webgl"),
                                             data.vertex,
                                             data.fragment );

      for(name in info.uniformUISpec) {
        var metaData = info.uniformUISpec[name];
        if("default" in metaData) {
          // could provide automatic defaults
          daliShader.registerAnimatedProperty(name, metaData.default);
        }
      }

      if(daliShader) {
        this.shadersCreated[name] = data;
        this.shadersCreated[name].daliShader = daliShader;
      }
      ret = daliShader;
    } else {
      console.log("Requested shader could not be compiled:" + name);
    }

    return ret;
  }
};

/**
 * Get info for a shader. See getInfo()
 *
 * @return  {Object} dali.Shader
 */

UIShaderTab.prototype.getShaderInfo = function(name) {
  "use strict";
  var data = database.readShader(name);
  return this.getInfo(data);
};

/**
 * Rebuild UI tab drop down selection.
 *
 * @param {Object} Database
 */
UIShaderTab.prototype.rebuildDropdown = function(db) {
  "use strict";
  rebuildDropdown( document.getElementById("shaderDropDown"),
                   db,
                   "shaders",
                   function(name) {
                     return ["javascript:uiShaderTab.loadShader(\"" + name + "\")",
                             name];
                   }
                 );
};


/**
 * Get the UI tab data with vertex/fragment strings.
 *
 * @return {Object} shader data object
 */
UIShaderTab.prototype.getData = function() {
  "use strict";
  var e = ace.edit("editorVertex");
  var ret = {};
  ret.name = $("#textShaderName").text().substr(this.bufferPrepend.length);
  ret.vertex = native2ascii(e.getSession().getValue());
  e = ace.edit("editorFragment");
  ret.fragment = native2ascii(e.getSession().getValue());

  ret.hints = "";
  if( $("#requiresSelfDepthTest")[0].checked ) {
    ret.hints += " requiresSelfDepthTest";
  }
  if( $("#outputIsTransparent")[0].checked ) {
    ret.hints += " outputIsTransparent";
  }
  if( $("#outputIsOpaque")[0].checked ) {
    ret.hints += " outputIsOpaque";
  }
  if( $("#modifiesGeometry")[0].checked ) {
    ret.hints += " modifiesGeometry";
  }

  return ret;
};

/**
 * Get dali shader hints from hints string.
 *
 * @param {string} hints string (space separated)
 * @return  {integer} or'd hints to pass to Dali
 */
UIShaderTab.prototype.getDaliShaderHints = function(hintsString) {
  "use strict";
  var ret = 0;

  if(hintsString.search("requiresSelfDepthTest") >= 0) {
    ret += dali.ShaderHints.REQUIRES_SELF_DEPTH_TEST;
  }
  if(hintsString.search("outputIsTransparent") >= 0) {
    ret += dali.ShaderHints.OUTPUT_IS_TRANSPARENT;
  }
  if(hintsString.search("outputIsOpaque") >= 0) {
    ret += dali.ShaderHints.OUTPUT_IS_OPAQUE;
  }
  if(hintsString.search("modifiesGeometry") >= 0) {
    ret += dali.ShaderHints.MODIFIES_GEOMETRY;
  }
  return ret;
};

/**
 * Get information on a shader
 * If hasError==true then attributes etc will be empty. ie it must compile.
 *
 * Compile the shader with webGL. Query webGL for meta data.
 *
 * @param {object} data object. {vertex:"", fragment:""}
 * @return  {object} info metadata object
 *  // var info = {
 *  //   attributes: [],
 *  //   uniforms: [],
 *  //   attributeCount: 0,
 *  //   uniformCount: 0,
 *  //   hasError: false,    // compiles without error
 *  //   vertexError: "",
 *  //   fragmentError: "",
 *  //   linkError: ""
 *  // };
 */


// uniform vec3 uMe; // gui:number, min:0, max:1, default: 0.5
// uniform vec3 uMe; // gui:slider, min:0, max:1, default: 0.5
// uniform vec3 uMe; // gui:color, min:[0,0,0,0] max:[1,1,1,1], default: [0,0,0,1]

UIShaderTab.prototype.getInfo = function(data) {
  "use strict";

  var canvas = document.getElementById("canvas");

  var info = new dali.ShaderInfo();

  return info.fromCompilation(canvas.getContext("webgl"),
                              data.vertex,
                              data.fragment);
};

/**
 * Check a shader can compile. Shader given as data object with vertex/fragment buffers.
 *
 * Compile the shader with webGL as a preprocess step to giving it to Dali.
 *
 * @param {string} shader buffer name
 * @return  {Boolean} true if the shader compiles ok.
 */
UIShaderTab.prototype.isCompilable = function(data) {
  "use strict";
  var info = this.getInfo(data);
  return !info.hasError;
};


/**
 * Check the shader in the UI tab can compile and if so update the saved dali.Shader object.
 * (Does not create the dali.Shader object)
 *
 * For use on an event for continuous shader checking.
 *
 * @param {string} shader buffer name
 * @return  {Object} dali.Shader
 */
UIShaderTab.prototype.checkAndUpdateShader = function() {
  "use strict";
  if(this.inhibitCheckAndUpdateShader) {
    return;
  }

  var options = this.getData();

  var info = this.getInfo(options);

  if(!info.hasError) {
    if( options.name in this.shadersCreated ) {
      if("daliShader" in this.shadersCreated[options.name]) {
        // setting shader.program= doesn't seem to recompile the shader?
        // this.shadersCreated[options.name].daliShader.delete(); no delete; could be held elsewhere
        delete this.shadersCreated[options.name];
      }
    }
  }

  var vertexPrepend = "";
  var fragmentPrepend = "";

  var errors = "";
  var count;
  var editor;
  var description;
  var col;
  var line;
  var i;

  var textShaderErrors = $("#textShaderErrors");

  textShaderErrors.value = "";

  editor = ace.edit("editorVertex");
  if (info.hasError && info.vertexError) {
    editor.getSession().setOption("useWorker", false);
    textShaderErrors.value = "VERTEX:\n" + info.vertexError;
    errors = info.vertexError.split("\n");
    count = vertexPrepend.split("\n").length;
    for(i = 0; i < errors.length; i++) {
      if(errors[i]) {
        description = errors[i].split(":");
        col = Number(description[1]);
        try {
          line = Number(description[2]);
        } catch(e) {
          line = "unknown";
        }
        editor.getSession().setAnnotations([{row: line - count, column: col, text: errors[i], type: "error"}]);
      }
    }
  } else {
    editor.getSession().setOption("useWorker", true);
    editor.getSession().setAnnotations([]);
  }

  editor = ace.edit("editorFragment");
  if (info.hasError && info.fragmentError) {
    editor.getSession().setOption("useWorker", false);
    textShaderErrors.value += "FRAGMENT:\n" + info.fragmentError;
    errors = info.fragmentError.split("\n");
    count = fragmentPrepend.split("\n").length;
    for(i = 0; i < errors.length; i++) {
      if(errors[i]) {
        description = errors[i].split(":");
        col = Number(description[1]);
        try {
          line = Number(description[2]);
        } catch(e) {
          line = "unknown";
        }
        editor.getSession().setAnnotations([{row: line - count, column: col, text: errors[i], type: "error"}]);
      }
    }
  } else {
    editor.getSession().setOption("useWorker", true);
    editor.getSession().setAnnotations([]);
  }

  if(!info.hasError) {
    database.writeShader(options);
  }

};


UIShaderTab.prototype.showModalC = function() {
  "use strict";
  var i;
  var info = this.getData();

  var data = "#define MAKE_STRING(A)#A\n\n" +
        "std::string vertexShader = MAKE_STRING(\n";

  var lines = info.vertex.split("\n");
  for(i = 0; i < lines.length; i++) {
    data += lines[i] + "\n";
  }
  data += ");\n\n";

  data += "std::string fragShader = MAKE_STRING(\n";

  lines = info.fragment.split("\n");
  for(i = 0; i < lines.length; i++) {
    data += lines[i] + "\n";
  }
  data += ");\n\n";

  data += "ShaderEffect shaderEffect;\n" +
    "shaderEffect = ShaderEffect::New( vertexShader, fragmentShader,\n" +
    "                                ShaderEffect::ShaderHint( " + info.hints + " ) )\n";

  data = data.replace(/<([^ ]*)/g, "< $1"); // for loops are interpreted by browser as tags and dont print?

  uiApp.codeInformationBox( data );

  // data = data.replace(/</g, "&lt;");
  // data = data.replace(/>/g, "&gt;");

  // var myWindow = window.open("data:text/html," + encodeURIComponent(data));
  // //                             "_blank");  // , "width=00,height=100");
  // myWindow.focus();

};

UIShaderTab.prototype.showModalJS = function() {
  "use strict";
  var i;
  var info = this.getData();

  var data = "var shaderOptions = {vertex:\n";

  var lines = info.vertex.split("\n");
  for(i = 0; i < lines.length; i++) {
    data += "                     \"" + lines[i] + "\\n\" +\n";
  }
  data += "                     \"\",\n";

  data += "                     fragment:\n";
  lines = info.fragment.split("\n");
  for(i = 0; i < lines.length; i++) {
    data += "                     \"" + lines[i] + "\\n\" +\n";
  }
  data += "                     \"\",\n";

  data += "                     geometryType: \"" + info.geometryType + "\",\n";
  data += "                     geometryHints: \"" + info.geometryHints + "\"\n";
  data += "                     };\n";

  data = data.replace(/<([^ ]*)/g, "< $1"); // for loops are interpreted by browser as tags and dont print?
  // data = data.replace(/</g, "&lt;");
  // data = data.replace(/>/g, "&gt;");

  // var myWindow = window.open("data:text/html," + encodeURIComponent(data));
  // //                             "_blank");  // , "width=00,height=100");
  // myWindow.focus();

  var modalBody = $("#modalCodeInfoBody")[0];
  removeAllChildren(modalBody);
  modalBody.innerHTML = "<code><pre>" + data + "</pre></code>";
  $("#modalCodeInfo").modal("show");

};

UIShaderTab.prototype.showModalJSON = function() {
  "use strict";
  var info = this.getData();

  var hints = ""; // tbd

  var vertex = info.vertex.replace(/\n/g, "\\n");
  var fragment = info.fragment.replace(/\n/g, "\\n");

  var data = "{\"shader-effects\": {\n" +
        "  \"" + "effect" + "\": {\n" +
        "    \"program\": {\n" +
        "    \"vertexPrefix\": \"\",\n" +
        "    \"vertex\": \"" + vertex + "\",\n" +
        "    \"fragmentPrefix\": \"\",\n" +
        "    \"fragment\": \"" + fragment + "\"\n" +
        "    },\n" +
        "    \"geometry-hints\": \"" + hints + "\",\n" +
        "    \"grid-density\": \"" + "0" + "\",\n" +
        "    \"image\": {\n" +
        "      \"filename\": \"\"\n" +
        "    }\n" +
        "  }\n" +
        " }\n" +
        "}\n";

  data = data.replace(/<([^ ]*)/g, "< $1"); // for loops are interpreted by browser as tags and dont print?

  uiApp.codeInformationBox( data );

};

document.getElementById("btnRewriteDatabase").addEventListener("click", function(// e
) {
  "use strict";

  database.delete(
    function() {
      consoleLogSuccess("database.delete")();
      window.setTimeout(function () {
        window.location.reload(true); // forces a reload from the server
      }, 500);
    },
    consoleLogErrorEvent, // fail
    consoleLogErrorEvent // blocked
  );

});


window.setTimeout(initDB, 1000);


function updateStatistics(eventHandler){
  "use strict";

  var elem = $("#statistics")[0];

  var actor = eventHandler.touchedActor;

  var usedRootLayer = false;

  if(!actor) {
    usedRootLayer = true;
    actor = dali.stage.getRootLayer();
  }

  var rt = dali.stage.getRenderTaskList().getTask(0);

  var xy;
  try {
    xy = dali.screenToLocal(eventHandler.mouseX, eventHandler.mouseY,
                            actor, rt);
  } catch(err) {
    xy = [0, 0];
  }

  var screenxy = dali.worldToScreen(actor.position, rt);

  var name = "Actor";

  if(usedRootLayer) {
    name = "Root Actor";
  }

  var prec = 3;
  var px = "none";
  var py = "none";
  if(eventHandler.mouseDownPosition) {
    px = eventHandler.mouseDownPosition[0].toFixed(prec);
    py = eventHandler.mouseDownPosition[1].toFixed(prec);
  }

  elem.innerHTML = name + " (" + actor.getId() + ") '" + actor.name + "'" + "<br>" +
    "Screen:" + eventHandler.mouseX.toFixed(prec) + "," + eventHandler.mouseY.toFixed(prec) + "<br>" +
    name + " Screen To Local:" + xy[0].toFixed(prec) + "," + xy[1].toFixed(prec) + "<br>" +
    name + " To Screen:" + screenxy[0].toFixed(prec) + "," + screenxy[1].toFixed(prec) + "<br>" +
    name + " Drag dx/dy:" + eventHandler.dragDx.toFixed(prec) + "," + eventHandler.dragDy.toFixed(prec) + "<br>" +
    name + " pos:" + actor.position + "<br>"+
    name + " pos:" + px + "," + py + "<br>";

  if(usedRootLayer) { // dont delete eventHandler object
    actor.delete(); // wrapper
  }

  rt.delete(); // wrapper

}


$(".btn-toggle").click(function() {
  "use strict";
  $(this).find(".btn").toggleClass("active");

  if($(this).find(".btn-primary").size() > 0) {
    $(this).find(".btn").toggleClass("btn-primary");
  }

  $(this).find(".btn").toggleClass("btn-default");

  if(this.id === "loop") {
    var looping = $(this).find("#loopOn").hasClass("active");
    animationList[animationSelectionIndex].looping = looping;
    animationList[animationSelectionIndex].dirtyData = true;
  }

  if(this.id === "endAction") {
    var bake = $(this).find("#endBake").hasClass("active");
    if(bake) {
      animationList[animationSelectionIndex].endAction = "Bake";
    } else {
      animationList[animationSelectionIndex].endAction = "Discard";
    }
    animationList[animationSelectionIndex].dirtyData = true;
  }

});

//------------------------------------------------------------------------------
//
// Database; access to browser indexed db
//
//------------------------------------------------------------------------------
function Database() {
  "use strict";
  this.currentVersion = 1;
  this.name = "dalitoy";
  this.data = {};
  var self = this;

  this.copyDBStore = function(db, dbStoreName, andClose) {
    var tr = db.transaction([dbStoreName], "readonly");
    var store = tr.objectStore(dbStoreName);
    var cursor = store.openCursor();
    self.data[dbStoreName] = {};
    cursor.onsuccess = function(e) {
      var res = e.target.result; // another cursor
      if(res) {
        self.data[dbStoreName][res.key] = res.value;
        res.continue();
      } else {
        if(andClose) {
          db.close();
        }
      }
    };
    cursor.onerror = consoleLogErrorEvent;
  };

  this.initializeData = function(db) {
    self.copyDBStore(db, "javascript");
    self.copyDBStore(db, "images");
    self.copyDBStore(db, "shaders", true); // true to close db (last copy)
  };

  this.init();
}

Database.prototype.open = function() {
  "use strict";
  return window.indexedDB.open(this.name, this.currentVersion);
};

Database.prototype.delete = function(onsuccess, onerror, onblocked) {
  "use strict";
  var req = window.indexedDB.deleteDatabase(this.name);
  req.onsuccess = onsuccess;
  req.onerror = onerror;
  req.onblocked = onblocked;
};


Database.prototype.upgrade = function(db, oldVersion) {
  // force upgrade
  "use strict";
  if (oldVersion < 1) {
    // Version 1 is the first version of the database.
    var store = db.createObjectStore("shaders", {keyPath: "name"});
    var i;
    for(i = 0; i < shaderSources.length; i++) {
      store.put( shaderSources[i] );
    }

    store = db.createObjectStore("javascript", {keyPath: "name"});
    for(i = 0; i < javascriptSources.length; i++) {
      store.put( javascriptSources[i] );
    }

    store = db.createObjectStore("images");

    // default images in html page
    store.put( getStockImageData(1), "girl1" );
    store.put( getStockImageData(2), "funnyface" );
    store.put( getStockImageData(3), "ducks" );
    store.put( getStockImageData(4), "field" );
  }
};

Database.prototype.init = function() {
  "use strict";
  var self = this;

  if("indexedDB" in window) {
    var openRequest = this.open();

    openRequest.onupgradeneeded = function(event) {
      var db = event.target.result;
      database.upgrade(db, event.oldVersion);
      //self.initializeData(db);
    };

    openRequest.onsuccess = function(event) {
      var db = event.target.result;
      self.initializeData(db);
    };

    openRequest.onerror = consoleLogErrorEvent;

  }

};

Database.prototype.initTabs = function(jstab, imagetab, shadertab) {
  "use strict";
  jstab.rebuildDropdown(this);
  imagetab.rebuildDropdown(this);
  shadertab.rebuildDropdown(this);
};

Database.prototype.writeJavascript = function(data, callback) {
  "use strict";

  this.data.javascript[data.name] = data;

  var openRequest = this.open();
  var self = this;
  openRequest.onsuccess = function(event) {
    var db = event.target.result;
    var tr = db.transaction(["javascript"], "readwrite");
    var store = tr.objectStore("javascript");

    var ob = store.put(data);
    self.data.javascript[data.name] = data;

    ob.onsuccess = callback;
    // ob.onerror = errorCallback;
    ob.onerror = consoleLogErrorEvent;

    db.close();
  };

  openRequest.onerror = consoleLogErrorEvent;

};

Database.prototype.readJavascript = function(name) {
  "use strict";
  return this.data.javascript[name];
};

Database.prototype.readJavascriptNames = function() {
  "use strict";
  return this.getKeys("javascript");
};

Database.prototype.readKeys = function(dbStoreName, callback) {
  "use strict";

  var openRequest = database.open();

  openRequest.onsuccess = function(event) {
    var db = event.target.result;
    var tr = db.transaction([dbStoreName], "readonly");
    var store = tr.objectStore(dbStoreName);
    var cursor = store.openCursor();
    var keys = [];
    cursor.onsuccess = function(e) {
      var res = e.target.result; // another cursor
      if(res) {
        keys.push(res.key);
        res.continue();
      } else {
        callback(keys);
      }
    };
    cursor.onerror = consoleLogErrorEvent;
    db.close();
  };

};

Database.prototype.getKeys = function(dbStoreName) {
  "use strict";
  var l = [];
  var o = this.data[dbStoreName];
  for(var name in o) {
    l.push(name);
  }
  return l;
};

Database.prototype.writeShader = function(data) {
  "use strict";
  assert(data.name);

  this.data.shaders[data.name] = data;

  // write the indexDB too
  var openRequest = this.open();

  openRequest.onsuccess = function(event) {
    var db = event.target.result;
    var tr = db.transaction(["shaders"], "readwrite");
    var store = tr.objectStore("shaders");

    var ob = store.put(data);

    //ob.onsuccess = callback;
    ob.onerror = consoleLogErrorEvent;

    db.close();
  };

  openRequest.onerror = consoleLogErrorEvent;

};

Database.prototype.readShader = function(name) {
  "use strict";
  return this.data.shaders[name];
};

Database.prototype.readShaderNames = function() {
  "use strict";
  return this.getKeys("shaders");
};

Database.prototype.readObjectStore = function(objectStoreName, recordName, callback, errorCallback) {
  "use strict";
  var openRequest = window.indexedDB.open(this.name, this.currentVersion);

  openRequest.onsuccess = function(event) {
    var db = event.target.result;

    var transaction = db.transaction([objectStoreName], "readonly");
    var objectStore = transaction.objectStore(objectStoreName);

    //x is some value
    var ob = objectStore.get(recordName);

    ob.onsuccess = function(e) {
      // read with undefined is still a success (should probably do this with cursor?)
      if(e.target.result) {
        callback(e.target.result);
      } else {
        errorCallback();
      }
    };

    ob.onerror = errorCallback;

    db.close();
  };

  openRequest.onerror = errorCallback;

};

Database.prototype.writeImage = function(name, data) {
  "use strict";

  this.data.images[name] = data;

  var openRequest = this.open();

  openRequest.onsuccess = function(event) {
    var db = event.target.result;
    var tr = db.transaction(["images"], "readwrite");
    var store = tr.objectStore("images");

    var ob = store.put(data, name);

    //ob.onsuccess = callback;
    ob.onerror = consoleLogErrorEvent;

    db.close();
  };

  openRequest.onerror = consoleLogErrorEvent;

};

Database.prototype.readImage = function(name) {
  "use strict";
  return this.data.images[name];
};

Database.prototype.readImageNames = function() {
  "use strict";
  return this.getKeys("images");
};

//------------------------------------------------------------------------------
//
// app init functions
//
//------------------------------------------------------------------------------
function initDB() {
  "use strict";
  database = new Database();
  window.setTimeout(init, 500);
}

function init() {
  "use strict";

  // var root = dali.stage.getRootLayer();
  // root.name = "*" + root.name; // * at start means non selectable by convention
  // root.delete(); // wrapper

  // database = new Database();

  test = new Test();

  uiApp = new UIApp();

  uiJavascriptTab = new UIJavascriptTab();

  uiImageTab = new UIImageTab();

  uiShaderTab = new UIShaderTab();

  database.initTabs(uiJavascriptTab, uiImageTab, uiShaderTab);

  $("a[rel=popover]").tooltip();

  Object.defineProperty(dali, "shader", {
    enumerable: true,
    configurable: false,
    get: function() {
      return getShader();
    }
  });

  Object.defineProperty(dali, "actor", {
    enumerable: true,
    configurable: false,
    get: function() {
      return getActor();
    }
  });

  Object.defineProperty(dali, "animation", {
    enumerable: true,
    configurable: false,
    get: function() {
      return getAnimation();
    }
  });

  eventHandler.handlersMouseMove.push(updateStatistics);

}

////////////////////////////////////////////////////////////////////////////////////////////////////

dali.postRenderFunction = undefined;
