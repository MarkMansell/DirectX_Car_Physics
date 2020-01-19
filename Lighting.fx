Texture2D gTexture : register(t0);
Texture2D gTexture2; 

SamplerState gAnisoSampler: register(s0);

cbuffer cbData : register(b0)
{
	float4x4 World; 
	float4x4 View; 
	float4x4 Projection;

	float4 gDiffuseMtrl; 
	float4 gDiffuseLight; 
	float4 gLightVecW;
	float4 gSpec;

	float4 gAmbientMtrl;
	float4 gAmbientLight;

	float4 gSpecularMtrl;
	float4 gSpecularLight;
	float4 gEyePosW;

	float4 gTranparency;
};

struct VS_IN
{
	float4 posL   : POSITION;
	float3 normalL : NORMAL;
	float2 texL : TEXCOORD;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION; 
	float3 NormW : NORMAL;
	float3 PosW : POSITION;
	float2 TexC : TEXCOORD;
};

VS_OUT VS(VS_IN vIn)
{
	VS_OUT output = (VS_OUT)0;

	output.PosW = mul( vIn.posL, World ).xyz; 

	output.Pos = mul( vIn.posL, World );
	output.Pos = mul( output.Pos, View ); 
	output.Pos = mul( output.Pos, Projection );

	// Convert from local to world normal
	float3 normalW = mul(float4(vIn.normalL, 0.0f), World).xyz;
	normalW = normalize(normalW);
	output.NormW = normalW;

	output.TexC = vIn.texL;

	return output;
}

float4 PS(VS_OUT pIn) : SV_Target
{
	// Compute the reflection vector.
	float3 toEye = normalize(pIn.PosW - gEyePosW.xyz);

	float3 lightNorm = normalize(gLightVecW.xyz - pIn.PosW);

	float3 r = reflect(lightNorm, pIn.NormW);
	// Determine how much (if any) specular light makes it
	// into the eye.
	float t = pow(max(dot(r, toEye), 0.0f), gSpec.x);//HACK:: gSpec is 4 times larger than it should be since the constantbuffer only seems to define memory in base 2? 
	// Determine the diffuse light intensity that strikes the vertex
	float s = max(dot(lightNorm, pIn.NormW), 0.0f); 
	// Compute the ambient, diffuse, and specular terms separately.

	if (s <= 0.0f) //Specular only shows up on areas that have diffuse lighting 
	{
		t = 0.0f;
	}


	float4 tex = gTexture.Sample(gAnisoSampler, pIn.TexC); 
	float4 texSpec = gTexture2.Sample(gAnisoSampler, pIn.TexC); 

	//float3 spec = t*(gSpecularMtrl*gSpecularLight).rgb;
	float3 spec = t*(texSpec*gSpecularMtrl*gSpecularLight).rgb;
	float3 diffuse = s*(gDiffuseMtrl*gDiffuseLight).rgb; 
	float3 ambient = (gAmbientMtrl * gAmbientLight).rgb; 
	

	// Sum all the terms together and copy over the diffuse alpha.
	float4 Col;

	Col.rgb = ((ambient + diffuse) * tex.rgb) + spec; 
	Col.a = tex.a * gTranparency.x;

	return Col;
	//return (1.0f, 0.0f, 0.0f, 1.0f);
}

//technique11 Render
//{
//	pass P0
//	{
//		SetVertexShader( CompileShader( vs_4_0, VS() ) ); SetGeometryShader( NULL );
//		SetPixelShader( CompileShader( ps_4_0, PS() ) );
//	}
//}
