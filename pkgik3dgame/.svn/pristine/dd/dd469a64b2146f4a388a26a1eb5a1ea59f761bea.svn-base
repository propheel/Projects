sampler samplerState;

struct PShaderInput
{
	float2 TexCoord : TEXCOORD0;
};

float4 normalColors(struct PShaderInput input) : COLOR0
{
	return tex2D( samplerState, input.TexCoord );
}

float4 invertColors(struct PShaderInput input) : COLOR0
{
	return 1 - tex2D( samplerState, input.TexCoord );
}

float4 blackAndWhiteColors(struct PShaderInput input) : COLOR0
{
	float4 color = tex2D( samplerState, input.TexCoord );

	float value = (color.r + color.g + color.b) / 3;

	color.r = value; 
	color.g = value;
	color.b = value;

	return color;
}

float4 sepiaColors(struct PShaderInput input) : COLOR0
{
	float4 color = tex2D( samplerState, input.TexCoord );
 
    float4 outputColor = color;
    outputColor.r = (color.r * 0.393) + (color.g * 0.769) + (color.b * 0.189);
    outputColor.g = (color.r * 0.349) + (color.g * 0.686) + (color.b * 0.168);    
    outputColor.b = (color.r * 0.272) + (color.g * 0.534) + (color.b * 0.131);

	return outputColor;
}

technique Normal
{
	pass P0
	{
		PixelShader = compile ps_2_0 normalColors();
	}
}

technique Negative
{
	pass P0
	{
		PixelShader = compile ps_2_0 invertColors();
	}
}

technique BlackAndWhite
{
	pass P0
	{
		PixelShader = compile ps_2_0 blackAndWhiteColors();
	}
}

technique Sepia
{
	pass P0
	{
		PixelShader = compile ps_2_0 sepiaColors();
	}
}