//
// Created by Nemo li on 2024/3/4.
//

#include <rttr/registration>
#include <helpers/screen.h>
#include "effekseer_effect.h"
#include "EffekseerRendererGL/EffekseerRendererGL.h"

RTTR_REGISTRATION {
    rttr::registration::class_<EffekseerEffect>("EffekseerEffect")
            .constructor<>()(rttr::policy::ctor::as_raw_ptr);
}

EffekseerRenderer::RendererRef efkRenderer;

EffekseerEffect::EffekseerEffect() {
    // Create a manager of effects
    // エフェクトのマネージャーの作成
    efkManager = ::Effekseer::Manager::Create(8000);

    // Create a  graphics device
    // 描画デバイスの作成
    auto graphicsDevice = ::EffekseerRendererGL::CreateGraphicsDevice(
            ::EffekseerRendererGL::OpenGLDeviceType::OpenGL3);

    // Create a renderer of effects
    // エフェクトのレンダラーの作成
    efkRenderer = ::EffekseerRendererGL::Renderer::Create(graphicsDevice, 8000);

    // Sprcify rendering modules
    // 描画モジュールの設定
    efkManager->SetSpriteRenderer(efkRenderer->CreateSpriteRenderer());
    efkManager->SetRibbonRenderer(efkRenderer->CreateRibbonRenderer());
    efkManager->SetRingRenderer(efkRenderer->CreateRingRenderer());
    efkManager->SetTrackRenderer(efkRenderer->CreateTrackRenderer());
    efkManager->SetModelRenderer(efkRenderer->CreateModelRenderer());

    // Specify a texture, model, curve and material loader
    // It can be extended by yourself. It is loaded from a file on now.
    // テクスチャ、モデル、カーブ、マテリアルローダーの設定する。
    // ユーザーが独自で拡張できる。現在はファイルから読み込んでいる。
    efkManager->SetTextureLoader(efkRenderer->CreateTextureLoader());
    efkManager->SetModelLoader(efkRenderer->CreateModelLoader());
    efkManager->SetMaterialLoader(efkRenderer->CreateMaterialLoader());
    efkManager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());

    // Specify a position of view
    // 視点位置を確定
    viewerPosition = ::Effekseer::Vector3D(0.0f, 0.0f, 50.0f);

    // Specify a projection matrix
    // 投影行列を設定
    projectionMatrix.PerspectiveFovRH(90.0f / 180.0f * 3.14f, Screen::aspect_ratio(), 1.0f, 500.0f);

    // Specify a camera matrix
    // カメラ行列を設定
    cameraMatrix.LookAtRH(viewerPosition, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f),
                          ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f));

    // Load an effect
    // エフェクトの読込
    effect = Effekseer::Effect::Create(efkManager, u"../res/effect/Sample.efkefc");

    efkHandle = 0;
}

void EffekseerEffect::Render() {
    // Move the effect
    // エフェクトの移動
    efkManager->AddLocation(efkHandle, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f));

    // Set layer parameters
    // レイヤーパラメータの設定
    Effekseer::Manager::LayerParameter layerParameter;
    layerParameter.ViewerPosition = viewerPosition;
    efkManager->SetLayerParameter(0, layerParameter);

    // Update the manager
    // マネージャーの更新
    Effekseer::Manager::UpdateParameter updateParameter;
    efkManager->Update(updateParameter);

    // Update a time
    // 時間を更新する
    efkRenderer->SetTime(time);

    // Specify a projection matrix
    // 投影行列を設定
    efkRenderer->SetProjectionMatrix(projectionMatrix);

    // Specify a camera matrix
    // カメラ行列を設定
    efkRenderer->SetCameraMatrix(cameraMatrix);

    // Begin to rendering effects
    // エフェクトの描画開始処理を行う。
    efkRenderer->BeginRendering();

    // Render effects
    // エフェクトの描画を行う。
    Effekseer::Manager::DrawParameter drawParameter;
    drawParameter.ZNear = 0.0f;
    drawParameter.ZFar = 1.0f;
    drawParameter.ViewProjectionMatrix = efkRenderer->GetCameraProjectionMatrix();
    efkManager->Draw(drawParameter);

    // Finish to rendering effects
    // エフェクトの描画終了処理を行う。
    efkRenderer->EndRendering();

    time++;
}

void EffekseerEffect::OnDestroy() {
    efkRenderer.Reset();
    efkManager.Reset();
}

void EffekseerEffect::PlayEffect() {
    efkHandle = efkManager->Play(effect, 0, 0, 0);
}

void EffekseerEffect::StopEffect() {
    efkManager->StopEffect(efkHandle);
}

