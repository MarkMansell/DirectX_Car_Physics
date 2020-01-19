//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register( b0 ){
	matrix World;
	matrix View;
	matrix Projection;
	float time;
}

struct VS_OUTPUT{
    float4 Pos : SV_POSITION;
	float4 PosW : POSITION;
    float4 Color : COLOR0;
};

// Amplitudes
static float a[2] = {2.5f, 0.1f};
	
// Angular wave numbers.
static float k[2] = {1.0, 8.0f};
	
// Angular frequency.
static float w[2] = {1.0f, 12.0f};
	
// Phase shifts.
static float p[2] = {0.0f, 1.0f};

float SumOfRadialSineWaves(float x, float z)
{
	
	// Distance of vertex from source of waves (which we set
	// as the origin of the local space).
	float d = sqrt(x*x + z*z);
	
	// Sum the waves.
	float sum = 0.0f;
	for(int i = 0; i < 2; ++i)
		sum += a[i]*sin(k[i]*d - time*w[i] + p[i]);
		
	
	return sum;
}


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION, float4 Color : COLOR )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul( Pos, World );

    //output.Pos.y = SumOfRadialSineWaves(output.Pos.x, output.Pos.z);
    output.PosW = output.Pos;

    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Color = Color;

    return output;
}



//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    return input.Color;
}
