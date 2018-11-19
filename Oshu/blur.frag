uniform sampler2D texture;
uniform float alpha;

void main()
{
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
	pixel.a = alpha;

    gl_FragColor = pixel * gl_Color;
}
