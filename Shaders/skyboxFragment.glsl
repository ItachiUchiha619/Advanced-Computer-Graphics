 #version  330  core

 uniform  samplerCube  cubeTex;
 
 uniform  samplerCube  cubeTex1;
 uniform  float fade;

 in  Vertex {
     vec3  viewDir;
 } IN;

 out  vec4  fragColour;


 void  main(void)    {
	vec4 sky1 = texture(cubeTex ,normalize(IN.viewDir));
	vec4 sky2 = texture(cubeTex1,normalize(IN.viewDir));
	vec4 skyMix = mix(sky1, sky2, fade);
     	fragColour = skyMix;
 }