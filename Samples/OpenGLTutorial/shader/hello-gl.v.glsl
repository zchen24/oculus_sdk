#version 400

//attribute vec2 position;
//varying vec2 texcoord;

//const char* vertexShader =
//        "float2 EyeToSourceUVScale;                                                                \n"
//        "float2 EyeToSourceUVOffset;                                                               \n"
//        "void main(in float2 Position    : POSITION,    in float4 Color       : COLOR0,         \n"
//        "          in float2 TexCoord0   : TEXCOORD0,   in float2 TexCoord1   : TEXCOORD1,      \n"
//        "          in float2 TexCoord2   : TEXCOORD2,                                           \n"
//        "          out float4 oPosition  : SV_Position, out float4 oColor     : COLOR,          \n"
//        "          out float2 oTexCoord0 : TEXCOORD0,   out float2 oTexCoord1 : TEXCOORD1,      \n"
//        "          out float2 oTexCoord2 : TEXCOORD2)                                           \n"
//        "{                                                                                      \n"
//        // Scale them into ([0,0.5],[0,1]) or ([0.5,0],[0,1]) UV lookup space (depending on eye)
//        "    oTexCoord0  = EyeToSourceUVScale * TexCoord0 + EyeToSourceUVOffset;                      \n"
//        "    oTexCoord1  = EyeToSourceUVScale * TexCoord1 + EyeToSourceUVOffset;                      \n"
//        "    oTexCoord2  = EyeToSourceUVScale * TexCoord2 + EyeToSourceUVOffset;                      \n"
//        "    oPosition = float4(Position.xy, 0.5, 1.0);                                         \n"
//        "    oColor = Color.r;  /*For vignette fade*/                                           \n"
//        "}";


// eye data from Oculus SDK
uniform vec2 EyeToSourceUVScale;
uniform vec2 EyeToSourceUVOffset;

in vec2 position;
in vec3 Color;   // Color from Program
in vec2 Texcoord0;
in vec2 Texcoord1;
in vec2 Texcoord2;

out vec2 oTexcoord0;
out vec2 oTexcoord1;
out vec2 oTexcoord2;
out vec3 oColor;

void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
//    texcoord = position * vec2(0.5) + vec2(0.5);

//    oTexcoord0 = position * vec2(0.5) + vec2(0.5);
//    oTexcoord1 = position * vec2(0.5) + vec2(0.5);
//    oTexcoord2 = position * vec2(0.5) + vec2(0.5);

    oTexcoord0 = position * vec2(0.5) + EyeToSourceUVOffset;
    oTexcoord1 = position * vec2(0.5) + EyeToSourceUVOffset;
    oTexcoord2 = position * vec2(0.5) + EyeToSourceUVOffset;

    oColor = vec3(1.0, 1.0, 1.0);
}



