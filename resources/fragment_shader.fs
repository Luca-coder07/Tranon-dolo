#version 330

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

uniform sampler2D texture0;

uniform vec3 lightPos;       // Position de la lumière (torche)
uniform vec3 lightDir;       // Direction de la lumière (spotlight)
uniform float cutOff;        // cos(angle de coupure du cône)
uniform float outerCutOff;   // cos(angle extérieur pour atténuation)
uniform vec3 viewPos;        // Position caméra

out vec4 finalColor;

void main()
{
    vec3 norm = normalize(normal);
    vec3 lightDirToFrag = normalize(lightPos - fragPos);
    
    // Calcul de l'angle entre la direction lumière et la direction vers le
 fragment
    float theta = dot(lightDirToFrag, normalize(-lightDir));
    
    // Base du spotlight : si hors cone on met zéro éclairage
    float intensity = 0.0;
    if(theta > cutOff)
    {
        // Soft edges
        float epsilon = cutOff - outerCutOff;
        intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);
    }
    
    // Diffuse lighting simple
    float diff = max(dot(norm, lightDirToFrag), 0.0);
    vec4 texColor = texture(texture0, texCoord);
    
    vec3 diffuse = diff * intensity * vec3(1.0, 1.0, 0.8);  // lumière
 légèrement jaunie
    
    vec3 result = diffuse * texColor.rgb;
    
    finalColor = vec4(result, texColor.a);
}
