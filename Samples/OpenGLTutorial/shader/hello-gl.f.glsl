#version 400


// -------------------------------------
// Example Shader from Oculus


//"Texture2D Texture   : register(t0);                                                    \n"
//"SamplerState Linear : register(s0);                                                    \n"

//"float4 main(in float4 oPosition  : SV_Position, in float4 oColor : COLOR,              \n"
//"            in float2 oTexCoord0 : TEXCOORD0,   in float2 oTexCoord1 : TEXCOORD1,      \n"
//"            in float2 oTexCoord2 : TEXCOORD2)   : SV_Target                            \n"
//"{                                                                                      \n"
//// 3 samples for fixing chromatic aberrations
//"    float ResultR = Texture.Sample(Linear, oTexCoord0.xy).r;                           \n"
//"    float ResultG = Texture.Sample(Linear, oTexCoord1.xy).g;                           \n"
//"    float ResultB = Texture.Sample(Linear, oTexCoord2.xy).b;                           \n"
//"    return float4(ResultR * oColor.r, ResultG * oColor.g, ResultB * oColor.b, 1.0);    \n"
//"}";


uniform float fade_factor;
uniform sampler2D texture;  // texture from app (video frame)

in vec2 oTexcoord0;  // from vertex shader, vertex coordinate
in vec2 oTexcoord1;  // from vertex shader, vertex coordinate
in vec2 oTexcoord2;  // from vertex shader, vertex coordinate
in vec3 oColor;     // from vertex shader, fading

void main()
{
    // sample value
    float ResultR = texture2D(texture, oTexcoord0).r * oColor.r;
    float ResultG = texture2D(texture, oTexcoord1).g * oColor.g;
    float ResultB = texture2D(texture, oTexcoord2).b * oColor.b;

//    gl_FragColor = vec4(ResultR, ResultG, ResultB, 1.0) * fade_factor;
    gl_FragColor = vec4(ResultR, ResultG, ResultB, 1.0);
}
