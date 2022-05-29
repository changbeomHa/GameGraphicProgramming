//--------------------------------------------------------------------------------------
// File: PhongShaders.fxh
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//--------------------------------------------------------------------------------------

#define NUM_LIGHTS (2)

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
/*--------------------------------------------------------------------
  TODO: Declare texture array and sampler state array for diffuse texture and normal texture (remove the comment)
--------------------------------------------------------------------*/
Texture2D aTextures[2] : register(t0);
SamplerState aSamplers[2] : register(s0);

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangesEveryFrame

  Summary:  Constant buffer used for world transformation
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/

cbuffer cbChangeOnCameraMovement : register(b0)
{
    matrix View;
    float4 CameraPosition;
}

cbuffer cbChangeOnResize : register(b1)
{
    matrix Projection;
}

/*--------------------------------------------------------------------
  TODO: cbChangesEveryFrame definition (remove the comment)
//--------------------------------------------------------------------------------------*/
cbuffer cbChangesEveryFrame : register(b2)
{
    matrix World;
    float4 OutputColor;
    bool HasNormalMap;
};

cbuffer cbLights : register(b3) {
    float4 LightPositions[NUM_LIGHTS];
    float4 LightColors[NUM_LIGHTS];
}
/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Struct:   VS_PHONG_INPUT

  Summary:  Used as the input to the vertex shader
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: VS_PHONG_INPUT definition (remove the comment)
--------------------------------------------------------------------*/
struct VS_PHONG_INPUT
{
    float4 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
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
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
};

struct PS_LIGHT_CUBE_INPUT
{
    float4 Position : SV_POSITION;
};

PS_LIGHT_CUBE_INPUT VSLightCube(VS_PHONG_INPUT input)
{
    PS_LIGHT_CUBE_INPUT output = (PS_LIGHT_CUBE_INPUT)0;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    return output;
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
/*--------------------------------------------------------------------
  TODO: Vertex Shader function VSPhong definition (remove the comment)
--------------------------------------------------------------------*/
PS_PHONG_INPUT VSPhong(VS_PHONG_INPUT input)
{ 
    PS_PHONG_INPUT output = (PS_PHONG_INPUT)0;    
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.TexCoord = input.TexCoord;

    output.Normal = normalize(mul(float4(input.Normal, 1), World).xyz);

    output.WorldPosition = mul(input.Position, World);

    if (HasNormalMap) 
    { 
        output.Tangent = normalize(mul(float4(input.Tangent, 0.0f), World).xyz);        
        output.Bitangent = normalize(mul(float4(input.Bitangent, 0.0f), World).xyz); 
    }     
    return output; 
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
/*--------------------------------------------------------------------
  TODO: Pixel Shader function PSPhong definition (remove the comment)
--------------------------------------------------------------------*/
float4 PSPhong(PS_PHONG_INPUT input) : SV_Target
{ 
    float3 normal = normalize(input.Normal);    
    if (HasNormalMap)
    {   
        // Sample the pixel in the normal map.        
        float4 bumpMap = aTextures[1].Sample(aSamplers[1], input.TexCoord);
        // Expand the range of the normal value from (0, +1) to (-1, +1).        
        bumpMap = (bumpMap * 2.0f) - 1.0f;
        // Calculate the normal from the data in the normal map.        
        float3 bumpNormal = (bumpMap.x * input.Tangent) + (bumpMap.y * input.Bitangent) + (bumpMap.z * normal);
        // Normalize the resulting bump normal and replace existing normal        
        normal = normalize(bumpNormal);
    } 


    // Ambient + Diffuse + Specular = Phong Reflection
    float3 ambient = float3(0.1f, 0.1f, 0.1f);
    float3 diffuse = float3(0, 0, 0);
    float3 specular = float3(0, 0, 0);
    float3 viewDirection = normalize(input.WorldPosition - CameraPosition.xyz);
    for (uint i = 0; i < 2; ++i) {
        // lightDirection은 방향이다.
        // saturate(x) : x를 [0,1]범위로 클램프, 헬퍼 슬라이드 수식에서 max(lambertian, 0.0f)를 대체함
        // dot : 내적

        // 방향 : 광원 -> 물체
        // 그래픽스 계에서는 반대로 사용한다? 그래서 앞에 direction 변수 앞에는 음수처리를 해주는건가?
        float3 lightDirection = normalize(input.WorldPosition - LightPositions[i].xyz);
        diffuse += saturate(dot(normal, -lightDirection) * LightColors[i].xyz);
        //diffuse : 시그마(n 내적 l) * 빛

        // reflact(i, n) : 반사벡터 리턴
        float3 reflectDirection = reflect(lightDirection, normal);
        specular += pow(saturate(dot(-viewDirection, reflectDirection)), 20.0f) * LightColors[i]; // n제곱 => 강도? 질감의 차이가 있는듯
        //specular : 시그마(r 내적 v) * 빛
    }
    // txDiffuse : 넘겨진 텍스쳐를 담는 오브젝트?
    // samLinear 텍스쳐 검색을 위한 샘플러
    // input.TexCoord : 소스에서 지정한 텍스쳐의 좌표
    // specular : 반사광, diffuse : 분산광, ambient : 주변광
    return float4(saturate(ambient + diffuse + specular), 1.0f) * aTextures[0].Sample(aSamplers[0], input.TexCoord);

}
float4 PSLightCube(PS_LIGHT_CUBE_INPUT input) : SV_Target
{
    return OutputColor;
}