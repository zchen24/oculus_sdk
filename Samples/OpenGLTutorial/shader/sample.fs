#version 120

uniform float fadeFactor;
uniform sampler2D textures[2];

varying vec2 texcord;

void main()
{
	// gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);

	gl_FragColor = mix(
		texture2D(textures[0], texcord),
		texture2D(textures[1], texcord),
		fadeFactor
		);
}
