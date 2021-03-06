#extension GL_ARB_texture_rectangle : enable
varying vec3 cameraPosition;
uniform sampler3D volume_tex;
uniform vec3 vol_d;
uniform float zoffset, quality, threshold, density, dithering;		// Quality vars
uniform float azimuth, elevation, clipPlaneDepth;					//Clipping plane vars
//uniform sampler2DRect lutTexture; // Color lookup texture, as sampler2Drect, using non-normalized coords

struct Ray {
    vec3 Origin;
    vec3 Dir;
};

struct BoundingBox {
    vec3 Min;
    vec3 Max;
};

bool IntersectBox(Ray r, BoundingBox box, out float t0, out float t1)
{
    vec3 invR = 1.0 / r.Dir;
    vec3 tbot = invR * (box.Min-r.Origin);
    vec3 ttop = invR * (box.Max-r.Origin);
    vec3 tmin = min(ttop, tbot);
    vec3 tmax = max(ttop, tbot);
    vec2 t = max(tmin.xx, tmin.yz);
    t0 = max(t.x, t.y);
    t = min(tmax.xx, tmax.yz);
    t1 = min(t.x, t.y);
    return t0 <= t1;
}

vec3 p2cart(float azimuth,float elevation)
{
    float pi = 3.1415926;
    float x, y, z, k;
    float ele = -elevation * pi / 180.0;
    float azi = (azimuth + 90.0) * pi / 180.0;

    k = cos( ele );
    z = sin( ele );
    y = sin( azi ) * k;
    x = cos( azi ) * k;

    return vec3( x, z, y );
}

void main()
{
	vec3 clipPlane = p2cart(azimuth, elevation);

    vec3 minv = vec3(0.)+1./vol_d;
    vec3 maxv = vec3(1.)-1./vol_d;
    vec3 vec;
    vec3 vold = (maxv-minv)*vol_d;
    float vol_l = length(vold);
	//	color_acc is the accumulated color during each setp
    vec4 col_acc = vec4(0,0,0,0);
    vec3 zOffsetVec = vec3(0.0,0.0,zoffset/vold.z);
    vec3 backPos = gl_TexCoord[0].xyz;//*maxv+minv;
    vec3 lookVec = normalize(backPos - cameraPosition);


    Ray eye = Ray( cameraPosition, lookVec);
    BoundingBox box = BoundingBox(minv, maxv);

    float tnear, tfar;
    IntersectBox(eye, box, tnear, tfar);
    if (tnear < 0.15) tnear = 0.15;
    if(tnear > tfar) discard;

    vec3 rayStart = eye.Origin + eye.Dir * tnear;
    vec3 rayStop = eye.Origin + eye.Dir * tfar;

	// starting position of the ray
    vec3 dir = rayStop - rayStart;
    
    float len = length(dir);
    dir = normalize(dir);
	
	// Clipping
	if (clipPlaneDepth > -1.0) 
	{
		//render the clipped surface invisible
		gl_FragColor.a = 0.0;
		//or render the clipped surface black
        //gl_FragColor.rgb = vec3(0.0,0.0,0.0);
		
        //see if clip plane faces viewer
        bool frontface = (dot(dir , clipPlane) > 0.0);
        //next, distance from ray origin to clip plane
        float dis = dot(dir,clipPlane);
        if (dis != 0.0  )  dis = (-clipPlaneDepth - dot(clipPlane, rayStart.xyz-0.5)) / dis;
        if ((!frontface) && (dis < 0.0)) return;
        if ((frontface) && (dis > len)) return;
        if ((dis > 0.0) && (dis < len)) 
        {
            if (frontface) 
            {
                rayStart = rayStart + dir * dis;
            } 
            else 
            {
                rayStop =  rayStart + dir * dis; 
            }
		}	
	}	

	dir = rayStop - rayStart;
	vec = rayStart;
	float dl = length(dir);

    if(dl == clamp(dl,0.,vol_l)) {
        float steps = floor(length(vold * dir) * quality);
        vec3 delta_dir = dir/float(steps);
        vec4 color_sample;
        float aScale =  density/quality;
		//Random fraction to be added onto the ray starting position
		float random = 2.0*fract(sin(gl_FragCoord.x*12.9898 + gl_FragCoord.y*78.233)*43758.5453);
		// Adding random component to ray starting position, to reduce anti aliasing effects (Dithering)
		vec = rayStart + (delta_dir*(random*dithering));
		
        /* Raycast for LUT
        for(int i = 0; i < int(steps); i++)
        {
            color_sample = texture3D(volume_tex, vec + zOffsetVec).r;
            if(color_sample > threshold) {

				float oneMinusAlpha = 1. - col_acc.a;
				color_sample *= aScale;
				col_acc.rgb = mix(col_acc.rgb,  texture2DRect(lutTexture, vec2(256.0*color_sample,1.0)).rgb * color_sample, oneMinusAlpha);
				
				col_acc.a += color_sample * oneMinusAlpha;
				col_acc.rgb /= col_acc.a;
				if(col_acc.a >= 1.0) {
						break; // terminate if opacity > 1
				}
		}
            vec += delta_dir;
	}*/
		
		//Raycast no LUT
		// repeat while penetrating the volume, and until the opacity gets full
		for(int i = 0; i < int(steps); i++)
		{
			vec3 vecz = vec + zOffsetVec;
			if(vecz.z > maxv.z) vecz.z-=maxv.z;
			color_sample.a = texture3D(volume_tex, vecz).r;
			color_sample.rgb = texture3D(volume_tex, vecz).rgb;
			
			if(color_sample.a > threshold) {
				
				float oneMinusAlpha = 1. - col_acc.a;
				color_sample.a *= aScale;
				vec3 white = vec3(256.0);
				col_acc.rgb = mix(col_acc.rgb, color_sample.rgb * color_sample.a, oneMinusAlpha);
				col_acc.a += color_sample.a * oneMinusAlpha;
				col_acc.rgb /= col_acc.a;
				
						
				// terminate if opacity > 1
				if(col_acc.a >= 1.0) {
					break;
				}
			}
			vec += delta_dir;
//			col_acc.rgb	= vec3(texture3D(volume_tex, vecz).r);
		}

}

    // export the rendered color
    gl_FragColor = col_acc;
}