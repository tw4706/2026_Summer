Texture2D tex : register(t4); //描画する黒い画面(今のところは)
Texture2D noiseTex : register(t5); //ディゾルブ用のノイズの画像
SamplerState smp : register(s4);
SamplerState noiseSmp : register(s5); //画像のサンプリング

//描画から渡すための構造体
struct PSInput
{
    float4 pos : SV_Position; //画面の座標
    float4 color : COLOR0; //頂点座標の色
    float2 texCoord : TEXCOORD0; //UV座標
};

//フェードの割合を受け取るためのバッファ
cbuffer DissolveBuffer : register(b1)
{
    float4 dissolve;
};

float4 main(PSInput input) : SV_TARGET
{
    float4 color = tex.Sample(smp, input.texCoord);
    float noise = noiseTex.Sample(noiseSmp, input.texCoord).r;

    float progress = dissolve.x;
    float edgeWidth = max(dissolve.y, 0.0001f);

    //ノイズ値が進行度を下回ったときは削除
    float diff = noise - progress;
    if (diff < 0.0f)
    {
        discard;
    }

    //エッジをかけて消えているふちを光らせる
    float edge = 1.0f - smoothstep(0.0f, edgeWidth, diff);
    float3 edgeColor = float3(1.0f, 0.6f, 0.1f);

    //最終的な色を補間
    float3 finalColor = lerp(color.rgb, edgeColor, edge);
    
    float finalAlpha = max(color.a, edge);

    return float4(finalColor, finalAlpha);
}