 #version 330 core 
        layout(location=0) out vec4 color;
 in vec3 gl_FragCoord;
        
 uniform vec3 u_pos;
 uniform vec2 u_resolution;
 uniform vec2 u_lookRot;

 const float PI = 3.14159;
 const int MAX_STEPS = 1000;
 const float EPSILON = 0.001;

 vec3 rotate(vec3 origin, vec3 pos, vec2 rot)
 {
     rot *= -1.0;
     pos -= origin;
     pos = pos * mat3(vec3(1, 0, 0),
         vec3(0, cos(rot.x), -sin(rot.x)),
         vec3(0, sin(rot.x), cos(rot.x))
     );


     pos = pos * mat3(vec3(cos(rot.y), 0, sin(rot.y)),
         vec3(0, 1, 0),
         vec3(-sin(rot.y), 0, cos(rot.y))
     );

     pos += origin;
     return pos;
 }


 float sphereSDF(vec3 pos, vec3 sPos, float rad)
 {
     return length(sPos - pos) - rad;
 }

 vec2 sceneSDF(vec3 pos)
 {
     vec3 modPos;
     modPos.xy = mod(pos.xy, 4);
     modPos.z = abs(pos.z);
     float sDist = sphereSDF(modPos, vec3(2, 2, 10), 3.);
     float mDist = sphereSDF(pos, u_pos, 2);
     float dist = min(sDist,mDist);
     //dist = min(sphereSDF(pos, vec3(-2, 2, 10), 3.), dist);
     if (dist == sDist)return vec2(dist, 0);
     else return vec2(dist, 1);
 }

 vec3 ray(vec2 fragCoord, vec2 resolution, float fov)
 {
     return normalize(vec3(fragCoord - resolution * 0.5, resolution.y / tan(radians(fov) / 2.0)));
 }

 vec4 marchRay(vec3 pos, vec3 dir)
 {
     vec3 camPos = pos;
     int n = 0;
     while (n < MAX_STEPS)//&& length(pos - camPos) < 100.0
     {
         vec2 dist = sceneSDF(pos);

         if (dist.x < EPSILON)
         {
             return vec4(pos,dist.y);
         }

         pos += dir * dist.x;
         n++;
     }

     return vec4(0, 0, 0, 0);
 }

 vec3 normal(vec3 pos)
 {
     return normalize(vec3(
         sceneSDF(vec3(pos.x + EPSILON, pos.y, pos.z)).x - sceneSDF(vec3(pos.x - EPSILON, pos.y, pos.z)).x,
         sceneSDF(vec3(pos.x, pos.y + EPSILON, pos.z)).x - sceneSDF(vec3(pos.x - EPSILON, pos.y, pos.z)).x,
         sceneSDF(vec3(pos.x, pos.y, pos.z + EPSILON)).x - sceneSDF(vec3(pos.x, pos.y - EPSILON, pos.z)).x
     ));

 }

 vec3 difuse(vec3 ambient, vec3 oPos, float roughness, float id)
 {  
     if (id == 1.)
         ambient = vec3(0.07, 0.63, 0.80);
     vec3 relPos = u_pos-oPos + vec3(5, 0, 0);
     vec3 color = max(ambient * dot(normal(oPos), normalize(relPos)) * roughness, 0.0);
     color /= max(length(relPos)/2.0,1.);
     return color;
 }


 void main()
 {

     vec3 rayDir = ray(gl_FragCoord.xy, u_resolution, 120.0);

     rayDir = rotate(vec3(0), rayDir, vec2(0,0.5*PI));

     vec3 col;

     vec4 hitpos = marchRay(u_pos+vec3(5,0,0), rayDir);
     if (hitpos != vec4(0))
         col = difuse(vec3(0.29, 0.34, 0.36), hitpos.xyz, 1.2, hitpos.w);
     else
         col = vec3(0, 0, 0);

     color = vec4(col, 1.0);
 }