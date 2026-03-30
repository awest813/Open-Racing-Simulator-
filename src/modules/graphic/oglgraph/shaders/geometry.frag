#version 330 core
layout(location=0) out vec4 fragColor;
layout(location=1) out vec4 brightColor;

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;
in vec2 fragTexCoord2;
in vec4 fragPosLightSpace;

uniform sampler2D texture1;
uniform sampler2D shadowMap;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 ambientColor;
uniform vec3 viewPos;
uniform vec4 matAmbient;
uniform vec4 matDiffuse;
uniform vec4 matSpecular;
uniform float matShininess;

float shadowCalculation(vec4 fragPosLS, vec3 normal, vec3 lightDirection) {
    vec3 projCoords = fragPosLS.xyz / max(fragPosLS.w, 0.0001);
    projCoords = projCoords * 0.5 + 0.5;
    if (projCoords.z <= 0.0 || projCoords.z >= 1.0) return 0.0;
    if (projCoords.x <= 0.0 || projCoords.x >= 1.0 ||
        projCoords.y <= 0.0 || projCoords.y >= 1.0) return 0.0;

    float currentDepth = projCoords.z;
    float ndotl = max(dot(normal, lightDirection), 0.0);
    float bias = max(0.00035, 0.0035 * (1.0 - ndotl));
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(float(x), float(y)) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    return shadow * (1.0 - smoothstep(0.75, 1.0, projCoords.z));
}

void main() {
    vec4 texColor = texture(texture1, fragTexCoord);
    if (texColor.a < 0.1) discard;

    vec3 albedo = texColor.rgb;
    vec3 norm = normalize(fragNormal);
    vec3 lDir = normalize(lightDir);
    vec3 viewDir = normalize(viewPos - fragPos);

    float horizon = clamp(norm.z * 0.5 + 0.5, 0.0, 1.0);
    vec3 ambientTint = mix(vec3(0.70, 0.72, 0.78), vec3(1.0, 0.98, 0.94), horizon);
    vec3 ambient = ambientColor * ambientTint * mix(vec3(0.45), matAmbient.rgb, 0.55) * albedo;

    float diff   = max(dot(norm, lDir), 0.0);
    vec3 diffuse = diff * lightColor * matDiffuse.rgb * albedo;

    vec3 halfDir = normalize(lDir + viewDir);
    float shininess = clamp(matShininess > 0.0 ? matShininess : 32.0, 8.0, 128.0);
    float spec   = pow(max(dot(norm, halfDir), 0.0), shininess);
    vec3 specular = spec * lightColor * mix(vec3(0.04), matSpecular.rgb, 0.85) * (0.35 + 0.65 * diff);
    float fresnel = pow(1.0 - max(dot(norm, viewDir), 0.0), 5.0);
    vec3 rim = fresnel * 0.04 * lightColor * albedo;

    float shadow = shadowCalculation(fragPosLightSpace, norm, lDir);

    vec3 lit = ambient + (1.0 - shadow) * (diffuse + specular) + rim;
    float distanceToCamera = length(viewPos - fragPos);
    float fogFactor = clamp(1.0 - exp(-distanceToCamera * 0.0007), 0.0, 0.35);
    vec3 fogColor = mix(ambientColor * 1.6, lightColor, 0.25);
    vec3 result = mix(lit, fogColor, fogFactor);

    fragColor = vec4(result, texColor.a);

    float luminance = dot(result, vec3(0.2126, 0.7152, 0.0722));
    float bloomMask = smoothstep(0.9, 1.6, luminance);
    brightColor = vec4(result * bloomMask, 1.0);
}
