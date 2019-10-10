#version 330 core
out vec4 fragColor;

in vec3 normal;
in vec2 uv;
in vec3 FragPos;  

uniform sampler2D uvmap;
uniform samplerCube depthmap;


uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float far_plane;

float ShadowCalculation(vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);
//    float shadow = 0.0;
    float bias = 0.15;
//    int samples = 20;
//    float viewDistance = length(viewPos - fragPos);
//    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
//    for(int i = 0; i < samples; ++i)
//    {
//        float closestDepth = texture(depthmap, fragToLight ).r;
//        closestDepth *= far_plane;   // undo mapping [0;1]
//        if(currentDepth - bias > closestDepth)
//            shadow += 1.0;
//    }
//    shadow /= float(samples);
        
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    


	float shadow = 0.0;
	float counter=0;
	for(float x = -1; x <= 1; x+=.5)
	{
		for(float y = -1; y <= 1; y+=.5)
		{
			for(float z = -1; z <= 1; z+=.5)
			{
				float pcfDepth = texture(depthmap, fragToLight + vec3(x, y, z)/10. ).r * far_plane; 
				shadow += currentDepth - bias > pcfDepth ? .0 : 1.0;    
				counter+=1.;
			}
		}    
	}
	shadow /= counter;
        
    return shadow;
}
void main(){



	float dist=length(FragPos-lightPos);
	float visibility = clamp(ShadowCalculation(FragPos),0,1)*.25;
//	float intensity=clamp(dot(normal,lightPos),0.,1.);
//	fragColor=texture(uvmap,uv)*(.5*intensity+.1+visibility*.4)*5./dist;

	float attun= 1./dist;

    vec3 ambient = vec3(vec3(.5) * texture(uvmap,uv).rgb);

	vec3 shadow = vec3(1,.2,.2)*visibility;
  	
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse =  (diff * texture(uvmap,uv).rgb);
    
    vec3 viewDir = normalize(viewPos - FragPos);
//    vec3 reflectDir = reflect(-lightDir, norm);
	vec3 reflectDir = normalize(lightDir + viewDir);
    float spec = pow(clamp(dot(normal, reflectDir), 0.,1.), 5);
    vec3 specular = vec3(.1)*spec;  
        
    fragColor = vec4((ambient + specular + diffuse )*(attun+visibility),1);
}