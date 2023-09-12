// Mary's attempt at an Obra Din two tone toon-ish shader

Shader "Unlit/MaryToonShader"
{
    Properties
	{
        // we are only using two colors here - no texture
		_Color1("Color 1", Color) = (1, 1, 1, 1)
        _Color2("Color 2", Color) = (0.5, 0.65, 1, 1)
	}
	SubShader
	{
		Pass
		{
            // these are Pass Tags, both used for our specific lighting
            Tags
            {
                // requesting some lighting data to be passed to our shader
                "LightMode" = "ForwardBase"
                // restrict the data to only the main directional light
                "PassFlags" = "OnlyDirectional"
            }

			CGPROGRAM
			#pragma vertex vert
			#pragma fragment frag
			
			#include "UnityCG.cginc"
            #include "Lighting.cginc"

            // I don't need a sampler or tex since I'm not using a texture

            // MeshData contains the information stored within each vertex
			struct MeshData
			{
				float4 vertex : POSITION;				
				float4 uv : TEXCOORD0;
                float3 normal : NORMAL;
			};

            // FragInput is the input for the fragment shader 
            // and must be manually populated in the vertex shader
			struct FragInput
			{
				float4 pos : SV_POSITION;
				float2 uv : TEXCOORD0;
                float3 worldNormal : NORMAL;
			};
			
			FragInput vert (MeshData v)
			{
				FragInput o;
				o.pos = UnityObjectToClipPos(v.vertex);
				//o.uv = TRANSFORM_TEX(v.uv, _MainTex);
                // transform the normal from object space to world space
                o.worldNormal = UnityObjectToWorldNormal(v.normal);
				return o;
			}
			
			float4 _Color1;
            float4 _Color2;

			float4 frag (FragInput i) : SV_Target
			{
                float3 normal = normalize(i.worldNormal);
                float NdotL = dot(_WorldSpaceLightPos0, normal);
                // I want to further define the ranges for the toon shader
                float lightIntensity = smoothstep(0, 0.01, NdotL);
                if (NdotL <= 0)
                {
                    // we are in complete shadow
                    lightIntensity = 0;
                }
                else if (NdotL > 0 && NdotL <= 0.2)
                {
                    lightIntensity = 0.15;
                }
                else if (NdotL > 0.2 && NdotL <= 0.4)
                {
                    lightIntensity = 0.45;
                }
                else if (NdotL > 0.4 && NdotL <= 0.5)
                {
                    lightIntensity = 0.75;
                }
                else 
                {
                    lightIntensity = 1;
                }
                //if (NdotL > ) 
                // this light makes our shader effected by the color of the light
                float4 light = lightIntensity * _LightColor0;

				return _Color1 * (_Color2 + light);
			}
			ENDCG
		}
	}
}
