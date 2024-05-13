varying vec4 vert_pos;

uniform sampler2D texture;

void main()
{
    // lookup the pixel in the texture
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

    vec2 delta = vert_pos.xy - vec2(0, 0); // light source in (0, 0)
    float delta_len_squared = dot(delta, delta);
//    float delta_len = length(delta);

    float intensity = clamp(1.0 / (delta_len_squared + 1.1), 0.0, 1.0); // + 1.1 is important

    // multiply it by the color
    gl_FragColor = gl_Color * pixel * intensity;
}