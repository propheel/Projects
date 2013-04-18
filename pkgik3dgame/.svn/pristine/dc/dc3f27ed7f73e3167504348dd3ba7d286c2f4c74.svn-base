float4x4	matWorldViewProj;
float4x4	matInverseWorld;
float4		vLightDirection;
float4		vecLightDir;
float4		vecEye;
float4		vDiffuseColor;
float4		vSpecularColor;
float4		vAmbientColor;

texture ReflectionCubeMap;
samplerCUBE ReflectionCubeMapSampler = sampler_state 
{ 
    texture = <ReflectionCubeMap>;     
};

struct OUT
{
	float4 Pos	: POSITION;
	float3 L	: TEXCOORD0;
	float3 N	: TEXCOORD1;
	float3 V	: TEXCOORD2;
};

OUT VShader( float4 Pos: POSITION, float3 N: NORMAL )
{
	OUT Out = (OUT) 0;
	Out.Pos = mul(Pos, matWorldViewProj);
	Out.L = normalize(vLightDirection);
	Out.N = normalize(mul(matInverseWorld, N));
	Out.V = vecEye - Pos;
	
	return Out;
}

float4 PShader(float3 L: TEXCOORD0, float3 N: TEXCOORD1, float3 V: TEXCOORD2) : COLOR
{
	float3 ViewDir = normalize(V); 
	
	// Calculate normal diffuse light.
	float Diff = saturate(dot(N, L)); 

	// Calculate reflection vector and specular
	float3 Reflect = reflect(L,N);
    float Specular = pow(saturate(dot(Reflect, ViewDir)), 128); // R.V^n
    
    // use reflection vector to lookup in the cube map
    float3 ReflectColor = texCUBE(ReflectionCubeMapSampler, Reflect);

	// return the color
    return vAmbientColor*float4(ReflectColor,1) + vDiffuseColor * Diff*float4(ReflectColor,1) + vSpecularColor * Specular*float4(ReflectColor,1); 

}

technique Reflection
{
	pass P0
	{
		VertexShader = compile vs_2_0 VShader();
		PixelShader = compile ps_2_0 PShader();
	}
}