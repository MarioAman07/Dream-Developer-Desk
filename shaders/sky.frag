#version 330 core
// sky.frag - procedural sky seen through the window: day/night colour gradient,
// sun or moon disc, stars and a city skyline with lit windows at night. It uses
// the direction from the camera, so the sky looks infinitely far away.

in vec3 vWorldPos;
out vec4 FragColor;

uniform vec3 uViewPos;
uniform vec3 uSunDir;        // direction towards the sun (the moon at night)
uniform vec3 uSunColor;
uniform vec3 uZenith;        // sky colour straight up
uniform vec3 uHorizon;       // sky colour at the horizon
uniform float uStars;        // 0 by day, 1 at night
uniform float uDay;          // 0 night, 1 day
uniform float uTime;

// Pseudo-random value in [0, 1), the same formula as lattice() in our Assignment 4.
float random(vec2 p)
{
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    vec3 dir = normalize(vWorldPos - uViewPos);
    float elev = dir.y;                        // height above the horizon (-1..1)
    float azimuth = atan(dir.z, -dir.x);       // 0 = straight out of the window

    // Gradient from the horizon to the zenith.
    vec3 sky = mix(uHorizon, uZenith, clamp(elev * 1.4 + 0.1, 0.0, 1.0));

    // Sun / moon disc with a soft glow: the same pow(cos, n) as a specular highlight.
    float sd = max(dot(dir, normalize(uSunDir)), 0.0);
    sky += uSunColor * (pow(sd, 900.0) * 2.0 + pow(sd, 18.0) * 0.25 * uDay);

    // Stars: the sky is cut into a grid of cells (azimuth, elevation); some
    // cells have one small round star at a random place inside the cell.
    vec2 starUV = vec2(azimuth, elev) * 90.0;
    vec2 cell = floor(starUV);
    vec2 centre = vec2(random(cell + 3.1), random(cell + 5.7)) * 0.6 + 0.2;
    float dist = length(fract(starUV) - centre);
    float star = step(0.93, random(cell)) * (1.0 - smoothstep(0.0, 0.09, dist));
    float twinkle = 0.6 + 0.4 * sin(uTime * 3.0 + random(cell + 7.0) * 40.0);
    sky += vec3(star * twinkle * uStars * 1.5 * smoothstep(0.02, 0.15, elev));

    // Distant city: the height of the buildings changes in steps along the
    // azimuth; at night some windows are lit.
    float building = floor(azimuth * 14.0);
    float height = -0.05 + random(vec2(building, 7.0)) * 0.10 * step(0.3, random(vec2(building, 9.0)));
    if (elev < height) {
        vec3 wall = mix(vec3(0.08, 0.09, 0.12), uHorizon * 0.65, uDay);
        vec2 w = vec2(azimuth * 14.0 * 9.0, elev * 320.0);
        vec2 wf = fract(w);
        float isWindow = step(0.3, wf.x) * step(wf.x, 0.7) * step(0.35, wf.y) * step(wf.y, 0.65);
        float lit = step(0.6, random(floor(w) + building * 3.1));
        sky = wall + vec3(1.0, 0.75, 0.45) * isWindow * lit * (1.0 - uDay);
    } else if (elev < -0.06) {
        sky = mix(vec3(0.08), uHorizon * 0.6, uDay);   // ground
    }

    FragColor = vec4(min(sky, vec3(1.0)), 1.0);
}
