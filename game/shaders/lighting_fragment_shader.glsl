varying vec4 vert_pos;

uniform sampler2D texture;
uniform vec2 world_window_center;
uniform vec2 world_window_dimensions;

uniform vec2 world_light_positions[5];
uniform float light_intensities[5];

uniform float backgr_intensity;

float computeIntensity(float light_intens, vec2 light_world_pos)
{
    vec2 world_pos = vert_pos.xy * (world_window_dimensions / 2.0) + world_window_center;

    vec2 delta = world_pos.xy - light_world_pos;
    float delta_len_squared = dot(delta, delta);
    float intensity = clamp(light_intens / (delta_len_squared + 0.5), 0.0, 1.0);
    return intensity;
}

void main()
{
    // lookup the pixel in the texture
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

    float total_lights_intens = 0.0;
    for (int i = 0; i < 5; i++)
    {
        if (light_intensities[i] < 0.0f)
        {
            break;
        }

        float curr_intens = computeIntensity(light_intensities[i], world_light_positions[i]);
        total_lights_intens += curr_intens;
    }

    total_lights_intens = clamp(total_lights_intens, 0.0, 1.0);

    // multiply it by the color
    gl_FragColor = gl_Color * pixel * total_lights_intens * backgr_intensity;
}