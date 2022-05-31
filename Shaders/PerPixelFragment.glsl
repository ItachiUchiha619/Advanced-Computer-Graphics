#version  330  core

 uniform  sampler2D  diffuseTex;
 uniform  sampler2D  rTex;
 uniform  sampler2D  gTex;
 uniform  sampler2D  bTex;
 uniform  sampler2D  blendMap;
 uniform  vec3    cameraPos;
 uniform  vec4    lightColour;
 uniform  vec3    lightPos;
 uniform  float   lightRadius;

 in  Vertex {
     vec3  colour;
     vec2  texCoord;
     vec3  normal;
     vec3  worldPos;
 } IN;

 out  vec4  fragColour;

void  main(void)    {
    vec4 blendMapColour = texture(blendMap, IN.texCoord);
	
    float backTextureAmount = 1 - (blendMapColour.r + blendMapColour.g + blendMapColour.b);
   
    vec4 backgroundTextureColour = texture(diffuseTex,IN.texCoord) * backTextureAmount;
    vec4 rTextureColour = texture(rTex, IN.texCoord) * blendMapColour.r;
    vec4 gTextureColour = texture(gTex, IN.texCoord) * blendMapColour.g;
    vec4 bTextureColour = texture(bTex, IN.texCoord) * blendMapColour.b;

    vec4 totalColour = backgroundTextureColour + rTextureColour + gTextureColour + bTextureColour;
 
    vec4 diffuse = totalColour;

    
     vec3  incident   = normalize(lightPos  - IN.worldPos );
     vec3  viewDir    = normalize(cameraPos  - IN.worldPos );
     vec3  halfDir    = normalize(incident + viewDir );

    
     

 float  lambert      = max(dot(incident , IN.normal), 0.0f);
     float  distance     = length(lightPos  - IN.worldPos );
     float  attenuation = 1.0 - clamp(distance / lightRadius , 0.0,  1.0);

 float  specFactor   = clamp(dot(halfDir , IN.normal ) ,0.0 ,1.0);
     specFactor    = pow(specFactor , 500.0 );

  vec3  surface = (diffuse.rgb * lightColour.rgb);
    fragColour.rgb = surface * lambert * attenuation;
     fragColour.rgb += (lightColour.rgb * specFactor )* attenuation *0.33;
    fragColour.rgb +=  surface * 0.5f;    // ambient!
    fragColour.a = diffuse.a;
}