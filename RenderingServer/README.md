# レンダリングサーバ
3台のサーバで、それぞれ以下の4つのアプリケーションを動作させる。
* GameContractor：レンダリングサーバ内で他のアプリケーションのプロセス管理を行う。司令塔。
* EncodePiper：ゲームエンジンとSkyWay GAtewayの橋渡し。 
* GameEngine：MRコンテンツそのもの。今回はUnrealEngineを利用。
* [SkyWay Gateway](https://webrtc.ecl.ntt.com/documents/webrtc-gateway.html)：WebRTC通信エンジン。SkyWay様を利用。

## GameContractor
レンダリングサーバ内で常に起動している。  
APIと通信し、ユーザの接続が求められた時に、SkyWay Gateway、EncodePiperとGameEngineを起動、接続する。
また、ユーザ切断すると、対応するプロセスを終了する。

## EncodePiper
GameEngineのレンダリング結果を名前付きパイプで受信し、GStreamerでH264にエンコード、UDPでSkyWay Gatewayに送信。

## GameEngine
UnrealEngine内に3Dモデル、ライティング等の演出、レンダリング用のカメラを用意しておく。SkyWay Gateway経由で受信したクライアントの姿勢情報をカメラに反映、レンダリング結果をEncodePiperに送信する。
[./GameEngine/Source/MyProject3](./GameEngine/Source/MyProject3)に自分が書いたスクリプトがあります。C++で記述。