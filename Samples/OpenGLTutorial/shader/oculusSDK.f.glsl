#version 400
uniform float fadeFactor;
uniform sampler2D texture;  // texture from app (video frame)

in vec2 texcoord;

void main()
{
    gl_FragColor = texture2D(texture, texcoord);
}
