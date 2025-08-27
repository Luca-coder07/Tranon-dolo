#version 330

// Positions fournis par raylib
in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;

uniform mat4 mvp;
uniform mat4 matModel;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoord;

void main()
{
    fragPos = vec3(matModel * vec4(vertexPosition, 1.0));
    normal = mat3(transpose(inverse(matModel))) * vertexNormal;
    texCoord = vertexTexCoord;
    
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
