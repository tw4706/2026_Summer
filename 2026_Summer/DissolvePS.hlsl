Texture2D tex : register(t0);
Texture2D noiseTex : register(t1);
SamplerState smp : register(s0);

//描画から渡すための構造体
struct PSInput
{
    float4 pos : SV_Position;   //画面の座標
    float4 diff : COLOR0;       //ディフーズ
    float4 spc : COLOR1;        //スペキュラー
    float2 uv : TEXCOORD0;      //テクスチャ座標
    float2 suv: TEXCOORD1;      //サブテクスチャ座標
};

//フェードの割合を受け取るためのバッファ
cbuffer DissolveBuffer : register(b4)
{
    float value;
    float strength;
    float lightX;
    float lightY;
};

float4 main(PSInput input) : SV_TARGET
{
    float noise = noiseTex.Sample(smp, input.uv.xy).r;

    if (noise > value)
    {
        discard;
    }
    
    float4 texColor = tex.Sample(smp, input.uv.xy);
    
    float edgeDist = value - noise;
    
    if (edgeDist < strength && value < 1.0f) //完全に表示(1.0)されている時はエッジを出さない
    {
        //境界線に近いほど 1.0 に近づく比率を作成
        float edgeLerp = 1.0 - (edgeDist / strength);
        
        //エッジの発光
        float4 edgeColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
        
        //元の色とエッジのブレンド
        texColor = lerp(texColor, edgeColor, edgeLerp);
    }
    
    return texColor;
}