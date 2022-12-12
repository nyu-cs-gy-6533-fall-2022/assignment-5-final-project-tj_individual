#version 150 core

out vec4 outColor;
uniform sampler1D tex;

struct Sphere
{
    vec3 position;
    float radius;
    vec3 color;
};

float SphereIntersect(vec3 rayOrigin, vec3 rayDir, Sphere s, out vec3 intersectPos, out vec3 normal){
    vec3 rd = normalize(rayDir);
    vec3 co = rayOrigin - s.position;

    float b = dot(rd, co);
    float c = dot(co, co) - s.radius * s.radius;

    float disc = b * b -  c;
    
    if(disc < 0) 
        return -1.0;

    float t = - b - sqrt(disc);
    
    if(t < 0)
        return -1.0;
    
    intersectPos = rayOrigin + t * rd;
    normal = normalize(intersectPos - s.position);
    return t;
}

vec3 getColor(vec3 light, vec3 camera, Sphere s, vec3 min_intersectPos, vec3 min_normal){
		
        vec3 ambientColor = vec3(0.1);
        float specularEx = 10;
		//diffuse
        vec3 l = normalize(light - min_intersectPos); //light dir
        vec3 d = clamp(s.color * dot(min_normal, l), 0.0, 1.0);

        //specular
        //if not facing towards the light, skip
        if(dot(min_normal, l) <0)
            return clamp(ambientColor + d, 0.0f, 1.0f);
		
        vec3 r = 2 * dot(min_normal, l) * min_normal - l; //reflection dir
        vec3 v = normalize(camera - min_intersectPos); //observe dir
        vec3 spec = vec3(1.0f) * pow(dot(r,v), specularEx);

     	// part 3
		return clamp(ambientColor + d + spec, 0.0f, 1.0f);

		
	}

void main(){
    float x = float(texture(tex, (0+0.5)/4.0));
    float y = float(texture(tex, (1+0.5)/4.0));
    float z = float(texture(tex, (2+0.5)/4.0));
    float r = float(texture(tex, (3+0.5)/4.0));

    Sphere s = Sphere(vec3(x,y,z), r, vec3(1.0,0.5,0.0));
    vec3 camera = vec3(0.0);
    vec3 light = vec3(-1.9, 1.9, 0.0);
    vec3 p = vec3(gl_PointCoord.x,gl_PointCoord.y/4*3, -1);
    vec3 intersectPos;
    vec3 intersectNormal;
    float t = SphereIntersect(camera, p-camera,s, intersectPos, intersectNormal);
    


    if(t > 0 ){

        outColor = vec4(getColor(light, camera, s, intersectPos,intersectNormal),1.0);
    }
    else{
        outColor = vec4(0,0,0,1);
    }  
   
}