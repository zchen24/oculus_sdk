#version 400

in vec2 Position;
out vec2 texcoord;

void main()
{
    gl_Position = vec4(Position, 0.5, 1.0);
    texcoord = Position * vec2(0.5) + vec2(0.5);
}



