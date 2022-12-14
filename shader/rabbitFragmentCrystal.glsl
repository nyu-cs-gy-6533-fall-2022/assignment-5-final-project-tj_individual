
#version 150 core

#define M_PI 3.1415926535897932384626433832795
#define ICE_REFRACT_RATIO 1.31
#define SOFTSAMPLER 15
#define REFLECT_DEPTH 15
out vec4 outColor;

uniform sampler1D tex;
uniform int arrLen;
uniform vec3 light1;
uniform vec3 light2;
uniform vec3 light3;

uniform vec3 planePoint1;
uniform vec3 planeNormal1;

uniform vec3 planePoint2;
uniform vec3 planeNormal2;

uniform vec3 planePoint3;
uniform vec3 planeNormal3;

uniform vec3 planePoint4;
uniform vec3 planeNormal4;

uniform vec3 planePoint5;
uniform vec3 planeNormal5;

float random (in vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233)))*43758.5453123);
}

struct Material{
    vec3 color;
    float specularEx;
    bool isSpecular;
    bool isIce;
};

struct Triangle
{
    vec3 V[3];
    Material mat;
};

struct Plane{
    vec3 point;
    vec3 normal;
    Material mat;
};

Plane plane[5];
vec3 light[3];

Triangle getTriangle(int i){
    i *= 9;
     vec3 v[3];
        float a1 = float(texelFetch(tex, i,0));
        float a2 = float(texelFetch(tex, i+1,0));
        float a3 = float(texelFetch(tex, i+2,0));
        v[0] = vec3(a1, a2, a3);

        a1 = float(texelFetch(tex, i+3,0));
        a2 = float(texelFetch(tex, i+4,0));
        a3 = float(texelFetch(tex, i+5,0));
        v[1] = vec3(a1, a2, a3);

        a1 = float(texelFetch(tex,i+6,0));
        a2 = float(texelFetch(tex, i+7,0));
        a3 = float(texelFetch(tex, i+8,0));
        v[2] = vec3(a1, a2, a3);

        
        return Triangle( v, Material(vec3(0.67,0.82,1.0),10.0, true, true));
}

float PlaneIntersect(vec3 rayOrigin, vec3 rayDir, Plane plane, out vec3 intersectPos, out vec3 normal){
    vec3 rd = normalize(rayDir);
    float ln = dot(rd, plane.normal);
    if(ln == 0) 
        return -1.0f;
    float t = dot((plane.point - rayOrigin), plane.normal) / ln;

    if(t < 0)
        return -1.0f;

    intersectPos = rayOrigin + rd * t;
        
    if(ln>0)   
        normal = -plane.normal;
    else
        normal = plane.normal;

    return t; 
}

float TriangleIntersect(vec3 rayOrigin, vec3 rayDir, Triangle tri, out vec3 intersectPos, out vec3 normal, out bool isInsideObj){
        vec3 rd = normalize(rayDir);
        vec3 n = normalize(cross(tri.V[0]-tri.V[1], tri.V[1]-tri.V[2]));

        isInsideObj = dot(tri.V[0]-rayOrigin, n) < 0;

        float ln = dot(rd, n);
        if(ln == 0) 
            return -1.0;
        float t = dot((tri.V[0] - rayOrigin), n) / ln;
        // distance between two triangles
        if(t < 0.03)
            return -1.0;

        vec3 p = rayOrigin + rd * t;
        vec3 v0 = tri.V[2] - tri.V[0];
        vec3 v1 = tri.V[1] - tri.V[0];
        vec3 v2 = p - tri.V[0];

        float dot00 = dot(v0, v0);
        float dot01 = dot(v0, v1);
        float dot02 = dot(v0, v2);
        float dot11 = dot(v1, v1);
        float dot12 = dot(v1, v2);

        float invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
        float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
        float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

        if ((u >= 0) && (v >= 0) && (u + v < 1)){
            if(ln>0){
                normal = -n;
               
            }    
            else{
                normal = n;
              
            }
                
            intersectPos = p;
            return t; 
        } 
        else 
            return -1.0;
}

float ObjectIntersect(vec3 rayOrigin, vec3 rayDir, out vec3 min_intersectPos, out vec3 min_intersectNormal, out Material min_mat, out bool isInsideObj){
    float min_t = 99999;
    isInsideObj = false;
    //triangle intersections
    for(int i=0; i<arrLen/9; i++){
        vec3 intersectPos;
        vec3 intersectNormal;
        Triangle tri = getTriangle(i);
        float t = TriangleIntersect(rayOrigin, rayDir, tri, intersectPos, intersectNormal, isInsideObj);
        if(t>=0 && t<min_t){
            min_t = t;
            min_intersectPos = intersectPos;
            min_intersectNormal = intersectNormal;
            min_mat = tri.mat;
        }   
    }

    //plane intersections
    for(int i=0; i<5; i++){
        vec3 intersectPos;
        vec3 intersectNormal;
        float t = PlaneIntersect(rayOrigin, rayDir, plane[i], intersectPos, intersectNormal);
        if(t>=0 && t<min_t){
            min_t = t;
            min_intersectPos = intersectPos;
            min_intersectNormal = intersectNormal;
            min_mat = plane[i].mat;
        }   
    }

    return min_t;
}

bool pointInShadow(vec3 point, vec3 light){
    float d2light = length(light - point);   
    //triangle intersections
    for(int i=0; i<arrLen/9; i++){
        vec3 intersectPos;
        vec3 intersectNormal;
        bool temp;
        Triangle tri = getTriangle(i);
        float t = TriangleIntersect(point, light-point, tri, intersectPos, intersectNormal, temp);
        if(t>0.1 && t<d2light){
           return true;
        }   
    }

    //plane intersections
    for(int i=0; i<5; i++){
        vec3 intersectPos;
        vec3 intersectNormal;
        float t = PlaneIntersect(point, light-point, plane[i], intersectPos, intersectNormal);
         if(t>0.1 && t<d2light){
           return true;
        }  
    }

    return false;
}

float softShadowRatio(vec3 pos, vec3 lightVec) {
  float notInShadow = 0.0;
  int N = int(sqrt(SOFTSAMPLER));
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      // (i, j) represents tile locations
      // rand values are offsets in that grid, which has the range [0, 1/N]
      float x = float(i) / float(N) + random(vec2(0.0, sin(float(N))*10 ));
      float y = float(j) / float(N) + random(vec2(0.0, cos(float(N))*10 ));
      // convert to sphere coordinates
      float theta = 2.0 * M_PI * x;
      float phi = acos(2.0 * y - 1.0);
      float u = cos(phi);

      // find (x, y, z) offset
      vec3 sample = vec3(sqrt(1.0 - u*u)*cos(theta), sqrt(1.0 - u*u)*sin(theta), u);


      // apply sample offset to lightVec, check whether if the point is in shadow given the new light vec

      bool inShadow = pointInShadow(pos+ sample*0.08 , lightVec+sample*0.3 );
      notInShadow += float(!inShadow);
    }
  }
  return notInShadow/float(SOFTSAMPLER);

}


vec3 getReflect(vec3 rayOrigin, vec3 intersectPos, vec3 intersectNormal, bool isInsideObj){

    vec3 rayDir = normalize(intersectPos - rayOrigin);
    float eta = (isInsideObj) ? (1.0 / ICE_REFRACT_RATIO) : ICE_REFRACT_RATIO;
    float cos_theta_i = dot(rayDir, intersectNormal)/(length(rayDir) * length(intersectNormal));
    float theta_i = acos(cos_theta_i);
  


    // reflect
    if ((eta * sin(theta_i)) > 1.0) 
        return normalize(reflect(rayDir, intersectNormal));


       float theta_r = asin(eta * sin(theta_i));
    vec3 T = eta * rayDir - (eta * cos_theta_i + cos(theta_r)) * intersectNormal;

  return normalize(T);
    //return rayDir;
}


vec3 getColor( vec3 camera, Material mat, vec3 min_intersectPos, vec3 min_normal, bool noShadow){
        vec3 ambientColor = vec3(0);
        vec3 d_total = vec3(0);
        vec3 spec_total = vec3(0);
        vec3 kd = vec3(1,1,1);
        vec3 ks = vec3(1,1,1);

        for(int i=0; i<3; i++){
            

            float softRatio = softShadowRatio(min_intersectPos, light[i]);
          
            //diffuse
            vec3 l = normalize(light[i] - min_intersectPos); //light dir
            vec3 d = kd* mat.color * dot(min_normal, l);
            d_total += softRatio * d;

            //specular
            //if not facing towards the light, skip
            if(!mat.isSpecular || dot(min_normal, l) <0.1) 
                continue;

            vec3 r = -reflect(l, min_normal); //reflection dir
            //vec3 r = getReflect(camera, min_intersectPos, min_normal, false);
            vec3 v = normalize(camera - min_intersectPos); //observe dir
            vec3 spec = ks * pow(dot(r,v), mat.specularEx);        
            spec_total += softRatio * spec;
        }

		return clamp(ambientColor + d_total + spec_total, 0.0f, 1.0f);
	
}




void main(){

    vec3 camera = vec3(0.0);
    vec3 p = vec3(gl_PointCoord.x,gl_PointCoord.y/4*3, -1);

    
    light[0] = light1;
    light[1] = light2;
    light[2] = light3;
    
    plane[0] = Plane(planePoint1, planeNormal1, Material(vec3(1,0.25,0.25), 0.0, false, false));
    plane[1] = Plane(planePoint2, planeNormal2, Material(vec3(0.1,1.0,0.1), 0.0, false, false));
    plane[2] = Plane(planePoint3, planeNormal3, Material(vec3(0.2,0.2, 1.0), 0.0, false, false));
    plane[3] = Plane(planePoint4, planeNormal4, Material(vec3(1.0,0.5,1.0), 0.0, false, false));
    plane[4] = Plane(planePoint5, planeNormal5, Material(vec3(1.0,0.5,1.0), 0.0, false, false)); 

    //store ray hits history
    Material matHistory[REFLECT_DEPTH];
    vec3 intersectPosHistory[REFLECT_DEPTH];
    vec3 intersectNormalHistory[REFLECT_DEPTH];
    vec3 reflectDirHistory[REFLECT_DEPTH];
    int hits = 0;
    for(int i=0; i<REFLECT_DEPTH; i++){
        if(i==0){

            bool isInsideObj = false;
            float t = ObjectIntersect(camera, p-camera, intersectPosHistory[0], intersectNormalHistory[0], matHistory[0], isInsideObj);
            if(t <=0 || t >= 99999)
                break;
            hits++;
            if(!matHistory[0].isIce)
                break;
            reflectDirHistory[0] = getReflect(camera, intersectPosHistory[0], intersectNormalHistory[0], isInsideObj);

        } else{
            bool isInsideObj = false;
            float t = ObjectIntersect(intersectPosHistory[i-1], reflectDirHistory[i-1], intersectPosHistory[i], intersectNormalHistory[i], matHistory[i], isInsideObj);
            if(t <=0 || t >= 99999)
                break;
            hits++;
            if(!matHistory[i].isIce)
                break;
            reflectDirHistory[i] = getReflect(intersectPosHistory[i-1], intersectPosHistory[i], intersectNormalHistory[i], isInsideObj);
        }
                
    }


   
    //calculate outColor by ray hits history
    
    for(int i=hits-1; i>0; i--){
        
        vec3 reflectColor = getColor(intersectPosHistory[i-1], matHistory[i], intersectPosHistory[i], intersectNormalHistory[i], true);
        reflectColor = reflectColor / length(reflectColor) * length(matHistory[i-1].color) * 0.9;
        matHistory[i-1].color = 0.2 * matHistory[i-1].color + 0.8 * reflectColor;
    }
    vec3 result1 =  getColor(camera, matHistory[0], intersectPosHistory[0], intersectNormalHistory[0], false);
    vec3 result2 =  hits > int(REFLECT_DEPTH/5*4) ? vec3(0.0) :matHistory[hits-1].color * pow(0.9, hits);

    if(length(result1) < 0.01)
        outColor = vec4(result2, 1.0);
    else
        outColor = vec4(result1, 1.0);
}
