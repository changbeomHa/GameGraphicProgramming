//--------------------------------------------------------------------------------------
// File: SkinningShaders.fx
//
// Copyright (c) Microsoft Corporation.
//--------------------------------------------------------------------------------------
#define NUM_LIGHTS (2)

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
static const unsigned int MAX_NUM_BONES = 256u;
/*--------------------------------------------------------------------
  TODO: Declare a diffuse texture and a sampler state (remove the comment)
--------------------------------------------------------------------*/
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangeOnCameraMovement

  Summary:  Constant buffer used for view transformation
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: cbChangeOnCameraMovement definition (remove the comment)
--------------------------------------------------------------------*/
cbuffer cbChangeOnCameraMovement : register(b0)
{
    matrix View;
    float4 CameraPosition;
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangeOnResize

  Summary:  Constant buffer used for projection transformation
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: cbChangeOnResize definition (remove the comment)
--------------------------------------------------------------------*/
cbuffer cbChangeOnResize : register(b1)
{
    matrix Projection;
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangesEveryFrame

  Summary:  Constant buffer used for world transformation
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: cbChangesEveryFrame definition (remove the comment)
--------------------------------------------------------------------*/
cbuffer cbChangesEveryFrame : register(b2)
{
    matrix World;
    float4 OutputColor;
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbLights

  Summary:  Constant buffer used for shading
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: cbLights definition (remove the comment)
--------------------------------------------------------------------*/
cbuffer cbLights : register(b3)
{
    float4 LightPositions[NUM_LIGHTS];
    float4 LightColors[NUM_LIGHTS];
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbSkinning

  Summary:  Constant buffer used for skinning
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: cbLights definition (remove the comment)
--------------------------------------------------------------------*/
cbuffer cbSkinning : register(b4)
{
    matrix BoneTransforms[MAX_NUM_BONES];
};
//--------------------------------------------------------------------------------------
/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Struct:   VS_INPUT

  Summary:  Used as the input to the vertex shader
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: VS_INPUT definition (remove the comment)
--------------------------------------------------------------------*/
struct VS_INPUT
{
    float4 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    uint4 BoneIndices : BONEINDICES;
    float4 BoneWeights : BONEWEIGHTS;
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Struct:   PS_PHONG_INPUT

  Summary:  Used as the input to the pixel shader, output of the 
            vertex shader
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: PS_PHONG_INPUT definition (remove the comment)
--------------------------------------------------------------------*/
struct PS_PHONG_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 WorldPosition : WORLDPOS;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
/*--------------------------------------------------------------------
  TODO: Vertex Shader function VSPhong definition (remove the comment)
--------------------------------------------------------------------*/
PS_PHONG_INPUT VSPhong(VS_INPUT input) {
    PS_PHONG_INPUT output = (PS_PHONG_INPUT)0;

    matrix skinTransform = (matrix)0;    
    skinTransform += mul(input.BoneWeights.x, BoneTransforms[input.BoneIndices.x]);
    skinTransform += mul(input.BoneWeights.y, BoneTransforms[input.BoneIndices.y]);
    skinTransform += mul(input.BoneWeights.z, BoneTransforms[input.BoneIndices.z]);
    skinTransform += mul(input.BoneWeights.w, BoneTransforms[input.BoneIndices.w]);

    output.Position = mul(input.Position, skinTransform);  
    output.WorldPosition = mul(output.Position, World);
    output.Position = mul(output.Position, World);    
    output.Position = mul(output.Position, View);    
    output.Position = mul(output.Position, Projection);

    output.TexCoord = input.TexCoord;

    output.Normal = mul(float4(input.Normal, 0), skinTransform);
    output.Normal = normalize(mul(float4(output.Normal, 0), World).xyz);

    return output;
}
//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
/*--------------------------------------------------------------------
  TODO: Pixel Shader function PSPhong definition (remove the comment)
--------------------------------------------------------------------*/
float4 PSPhong(PS_PHONG_INPUT input) : SV_TARGET
{
       // Ambient + Diffuse + Specular = Phong Reflection
    float3 ambient = float3(0.1f,0.1f,0.1f);
    float3 diffuse = float3(0, 0, 0);
    float3 specular = float3(0, 0, 0);
    float3 viewDirection = normalize(input.WorldPosition - CameraPosition.xyz);    
    for (uint i = 0; i < 2; ++i) { 
        // lightDirection�� �����̴�.
        // saturate(x) : x�� [0,1]������ Ŭ����, ���� �����̵� ���Ŀ��� max(lambertian, 0.0f)�� ��ü��
        // dot : ����
        
        // ���� : ���� -> ��ü
        // �׷��Ƚ� �迡���� �ݴ�� ����Ѵ�? �׷��� �տ� direction ���� �տ��� ����ó���� ���ִ°ǰ�?
        float3 lightDirection = normalize(input.WorldPosition - LightPositions[i].xyz);
        diffuse += saturate(dot(input.Normal, -lightDirection) * LightColors[i].xyz);
        //diffuse : �ñ׸�(n ���� l) * ��
        
        // reflact(i, n) : �ݻ纤�� ����
        float3 reflectDirection = reflect(lightDirection, input.Normal);
        specular += pow(saturate(dot(-viewDirection, reflectDirection)), 20.0f) * LightColors[i]; // n���� => ����? ������ ���̰� �ִµ�
        //specular : �ñ׸�(r ���� v) * ��
    }
    // txDiffuse : �Ѱ��� �ؽ��ĸ� ��� ������Ʈ?
    // samLinear �ؽ��� �˻��� ���� ���÷�
    // input.TexCoord : �ҽ����� ������ �ؽ����� ��ǥ
    // specular : �ݻ籤, diffuse : �л걤, ambient : �ֺ���
    return float4(saturate(ambient + diffuse + specular), 1.0f) * txDiffuse.Sample(samLinear, input.TexCoord);
}