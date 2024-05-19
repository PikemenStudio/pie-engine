varying vec4 vert_pos;

uniform sampler2D texture;
uniform vec2 world_window_center;
uniform vec2 world_window_dimensions;
uniform vec2 world_light_pos;
uniform float light_intensity;

uniform float backgr_intensity;

void main()
{
    // lookup the pixel in the texture
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

    vec2 world_pos = vert_pos.xy * (world_window_dimensions / 2.0) + world_window_center;

    vec2 delta = world_pos.xy - world_light_pos; // for now, the light source is always at the center
    float delta_len_squared = dot(delta, delta);

//    float intensity = clamp(1.0 / (delta_len_squared + 1.1), 0.0, 1.0); // + 1.1 is important
//    float intensity = clamp(0.5 / (delta_len_squared + 0.5), 0.0, 1.0);
    float intensity = clamp(light_intensity / (delta_len_squared + 0.5), 0.0, 1.0);

    // multiply it by the color
    gl_FragColor = gl_Color * pixel * intensity * backgr_intensity;
}