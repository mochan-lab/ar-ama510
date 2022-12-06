# APIサーバ
接続ユーザを管理するためのAPIサーバ

## 動作環境
* Ubuntu20.04
* Apache2.4.41
* （以下はanaconda仮想環境下のpipにてinstall、ymlあり）
* fastapi
* uvicorn
* gunicorn  

Apacheからgunicornにプロキシしてfastapiを動作させる。