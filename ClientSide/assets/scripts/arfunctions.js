// DEFINE GLOBAL VARIABLE
var MySendStatus = {
    client: "undefined",
    rserver: 0,
    csend: 0.0,
    cstatus: 0,
    rstatus: 0,
    cloc: [0.0, 0.0, 0.0],  //X,Y,Z
    crot:[0.0,0.0,0.0]
};
var backtwocrot = [0.0,0.0,0.0];
var ReceivedStatus;

var MySetting={
    width: 0,
    height: 0,
    videowidth: 0,
    videoheight: 0,
    ua: "undetect",
    mpeerid: "testmedia",
    dpeerid: "testdata"
}

const peer = new Peer({
    key: 'SkyWay_APIKey',
    // Forcibly use TURN server.
    // config: {
    //     iceTransportPolicy: 'relay',
    // },
    debug: 3
})
var isIos;

var dataConnection=null;
var isStartSendUDP=false;
var mediaConnection=null;
var isAbsoluteSensor;   //some android can not use absolute heading angle
var populateMatrix=new Float32Array(16);

var relativeCount = 0;


// WITH PROMOTER API
const PromoterUrl = "APIServer_URL";
const RequestMakeClient=()=>{
    return new Promise((resolve,reject)=>{
        MySetting.width=innerWidth;
        MySetting.height=innerHeight;
        console.log([MySetting.width,MySetting.height]);
        if(MySetting.width==0 || MySetting.width==0){
            reject("NotDefine");
            return;
        }
        // iOS device's H264 decoder need image sizes are multiples of 16
        let pluwid = 16 - (MySetting.width % 16);
        let pluhei = 16 - (MySetting.width % 16); 
        let clientmeta = {
            clientwidth: MySetting.width + pluwid,
            clientheight: MySetting.height + pluhei,
            // clientwidth: (MySetting.width + pluwid) / 2,
            // clientheight: (MySetting.height + pluhei) / 2,
            useragent: MySetting.ua
        }
        $.ajax({
            type: "POST",
            url: PromoterUrl + "/client/connect",
            data: JSON.stringify(clientmeta),
            processData: false,
            contentType: "application/json",
            dataType: "json"
        })
        .fail(function(data, status, error){
            console.warn("error:" + data);
            reject("HTTPError");
            return;
        })
        .done(function(fdata){
            MySendStatus.client = fdata.client;
            if(fdata.status=="CANNOT_FIND_RSERVER"){
                let intervalCount=0;
                let continuelooking = setInterval(()=>{
                    if(intervalCount > 9){
                        clearInterval(continuelooking);
                        alert("接続できるサーバが見つかりませんでした。時間をおいてページを再読み込みしてください。");
                        reject("CanNotConnectRserver");
                        return;
                    }
                    intervalCount++;
                    $.ajax({
                        type: "GET",
                        url: PromoterUrl + "/client/" + fdata.client + "/lookingrserver",
                        dataType: "json"
                    })
                    .fail(function(data, status, error){
                        console.warn("error:" + error);
                        clearInterval(continuelooking);
                        alert("サーバとの接続が出来ませんでした。時間をおいてページを再読み込みしてください。");
                        reject("HTTPError");
                        return;
                    })
                    .done(function(fdata){
                        if(fdata.status=="LOOKING_FOR_RSERVER"){
                            clearInterval(continuelooking);
                            resolve("findRserver");
                            return;
                        }
                    })
                },1000)
            }else if(fdata.status=="LOOKING_FOR_RSERVER"){
                resolve("findRserver");
                return;
            }
        })
    })
}

const getCallee = () => {
    return new Promise((resolve,reject)=>{
        $.ajax({
            type: "GET",
            url: PromoterUrl + "/client/" + MySendStatus.client + "/waitingcallee",
            dataType: "json"
        })
        .fail(function(data, status, error){
            console.warn("error:" + error);
            reject("HTTPError");
            return;
        })
        .done(function(fdata){
            if(fdata.status=="NOW_CHECKING"){
                let intervalCount=0;
                let continuelooking = setInterval(()=>{
                    if(intervalCount > 9){
                        clearInterval(continuelooking);
                        alert("接続できるサーバが見つかりませんでした。時間をおいてページを再読み込みしてください。");
                        reject("CanNotReceivePeer");
                        return;
                    }
                    intervalCount++;
                    $.ajax({
                        type: "GET",
                        url: PromoterUrl + "/client/" + MySendStatus.client + "/waitingcallee",
                        dataType: "json"
                    })
                    .fail(function(data, status, error){
                        console.warn("error:" + error);
                        clearInterval(continuelooking);
                        alert("サーバとの接続が出来ませんでした。時間をおいてページを再読み込みしてください。");
                        reject("HTTPError");
                        return;
                    })
                    .done(function(fdata){
                        if(fdata.status=="SOON_CALL"){
                            clearInterval(continuelooking);
                            MySetting.dpeerid=fdata.dpeerid;
                            MySetting.mpeerid=fdata.mpeerid;
                            resolve("findpeers");
                            return;
                        }
                    })
                },1000)
            }else if(fdata.status=="SOON_CALL"){
                MySetting.dpeerid=fdata.dpeerid;
                MySetting.mpeerid=fdata.mpeerid;
                resolve("findpeers");
                return;
            }else if(fdata.status=="LOOKING_FOR_RSERVER"){
                let intervalCount=0;
                let continuelooking = setInterval(()=>{
                if(intervalCount > 9){
                    clearInterval(continuelooking);
                    alert("接続できるサーバが見つかりませんでした。時間をおいてページを再読み込みしてください。");
                    reject("CanNotConnectRserver");
                    return;
                }
                intervalCount++;
                $.ajax({
                    type: "GET",
                    url: PromoterUrl + "/client/" + fdata.client + "/lookingrserver",
                    dataType: "json"
                })
                .fail(function(data, status, error){
                    console.warn("error:" + error);
                    clearInterval(continuelooking);
                    alert("サーバとの接続が出来ませんでした。時間をおいてページを再読み込みしてください。");
                    reject("HTTPError");
                    return;
                })
                .done(function(fdata){
                    if(fdata.status=="LOOKING_FOR_RSERVER"){
                        clearInterval(continuelooking);
                        resolve("findRserver");
                        return;
                    }
                })
            },1000)}
        })
    })
}

const wantquit = () => {
    const url = PromoterUrl + "/client/" + MySendStatus.client + "/statusupdate";
    const senddata = {
        status: "WANT_QUIT"
    }
    const data = new Blob([JSON.stringify(senddata)], {
        type: "application/json",
    });
    navigator.sendBeacon(url,data);
}

// MRの位置合わせにVPSを利用する予定だったが、現地都合で最終的には利用せず。コード自体は動作確認済み。
// WITH IMMERSAL
var cameraIntrinsics = {x:0.0,y:0.0,z:0.0,w:0.0};
var encodedImage= null;
var isLocalizing=false;
var localizingCount=0;
const immersalUrl = "https://api.immersal.com";
const immersalToken="immersal_SDK_key";
const immersalMapId=0000; // your immersal map ID 
const flagRelativeLoc=[400.0, -100.0 , 30.0];
var relativeheading=0;

const degtorad = Math.PI / 180;

// IMMERSAL VPS FUNCTIONS
const localize=(cvs)=>{
    isLocalizing=true;
    let b64str = cvs.toDataURL('png/image').replace('data:image/png;base64,', '');
    let json={token: immersalToken, fx: cameraIntrinsics.x, fy: cameraIntrinsics.y, ox: cameraIntrinsics.z, oy: cameraIntrinsics.w, b64: b64str, mapIds: [{ id: immersalMapId}] };
    let jsonStr=JSON.stringify(json);
    let whensendcompass=MySendStatus.crot[1];
    $.ajax({
        type: "POST",
        url: immersalUrl + "/localizeb64",
        data: jsonStr,
        processData: false,
        dataType: "json"
    })
    .fail(function(data, status, error){
        console.warn("error:" + JSON.stringify(data.responseJSON));
        isLocalizing = false;
    })
    .done(function(data){
        if (data.success) {
            console.log("Relocalized successfully");
            localizingCount++;

            let ret = [0.0,0.0,0.0];
            ret[0] = data.px + flagRelativeLoc[0];
            ret[1] = data.px + flagRelativeLoc[1];
            ret[2] = data.px + flagRelativeLoc[2];
            MySendStatus.cloc = ret;

            relativeheading=(Math.atan2(data.r21,data.r22)/degtorad)-whensendcompass;
        }
        else {
            console.log("Failed to relocalize");
        }
        testText.innerText = localizingCount;
        isLocalizing = false;
    })
};

// WEBGL FUNCTIONS
// initWebGLについて、昼コンテンツと夜コンテンツでクロマキー合成のパラメータを変えた方が綺麗に合成出来たため、昼用と分かれている。
const initWebGL = ()=>{
    return new Promise((resolve, reject)=>{
        const scene = new THREE.Scene();
        const camera = new THREE.OrthographicCamera(MySetting.width / -2, MySetting.width / 2, MySetting.height / 2, MySetting.height / -2, 1, 1000);
        scene.add(camera);
        camera.position.z = 100;

        const renderer = new THREE.WebGLRenderer();
        renderer.setPixelRatio(window.devicePixelRatio);
        renderer.setSize(MySetting.width, MySetting.height, true);
        const glCanvas = document.body.appendChild(renderer.domElement);
        glCanvas.id="camera-canvas";
        gl=renderer.getContext();
        readbackFramebuffer=gl.createFramebuffer();

        const mycamera = document.getElementById('my-camera');
        const cameratex = new THREE.VideoTexture(mycamera);
        const cameramaterial = new THREE.SpriteMaterial({map:cameratex, color: 0xffffff});
        const camsprite = new THREE.Sprite(cameramaterial);
        camsprite.scale.set(MySetting.width + 10, MySetting.height + 10, 1);
        scene.add(camsprite);
        camsprite.position.set(0, 0, 5);

        const recvvideo = document.getElementById('receive-video');
        const recvvideotex = new THREE.VideoTexture(recvvideo);
        const chromamaterial = new THREE.ShaderMaterial({
            transparent: true,
            uniforms:{
                map:{value:recvvideotex},
                keyColor:{value:[0.0,1.0,0.0]},
                similarity:{value:0.6},
                // similarity:{value:0.5},
                smoothness:{value:0.2},
            },
            vertexShader:vertexShader(),
            fragmentShader:fragmentShader()
        })
        function vertexShader() {
            return `
                varying vec2 vUv;
                void main( void ) {     
                    vUv = uv;
                    gl_Position = projectionMatrix * modelViewMatrix * vec4(position,1.0);
                }
            `
        }
        function fragmentShader() {
            return `
                uniform vec3 keyColor;
                uniform float similarity;
                uniform float smoothness;
                varying vec2 vUv;
                uniform sampler2D map;
                void main() {
                    vec4 videoColor = texture2D(map, vUv);
                    float Y1 = 0.299 * keyColor.r + 0.587 * keyColor.g + 0.114 * keyColor.b;
                    float Cr1 = keyColor.r - Y1;
                    float Cb1 = keyColor.b - Y1;
                
                    float Y2 = 0.299 * videoColor.r + 0.587 * videoColor.g + 0.114 * videoColor.b;
                    float Cr2 = videoColor.r - Y2; 
                    float Cb2 = videoColor.b - Y2; 
                
                    float blend = smoothstep(similarity, similarity + smoothness, distance(vec2(Cr2, Cb2), vec2(Cr1, Cb1)));
                    gl_FragColor = vec4(videoColor.rgb, videoColor.a * blend); 
                }
            `
        }
        const recvsprite = new THREE.Sprite(chromamaterial);
        recvsprite.scale.set(MySetting.width +10, MySetting.height + 10, 1);
        scene.add(recvsprite);
        recvsprite.position.set(0, 0, 10);

        const animate = () => {
            renderer.render( scene, camera );
            if(MySendStatus.cstatus==-1 && !isLocalizing){ // it truly 1 !!
                localize(glCanvas);            
            }
            if(isStartSendUDP){
                // if(isStartSendUDP && sendCount>5){
                dataConnection.send(JSON.stringify(MySendStatus));
                // console.log(ret);
                // sendCount = 0;
            }
            // sendCount ++;
            window.requestAnimationFrame(animate);
        }
        window.requestAnimationFrame(animate);
        MySendStatus.cstatus=2; //complete setting
        resolve("startwebGL");
    })
}

const initWebGLDay = ()=>{
    return new Promise((resolve, reject)=>{
        const scene = new THREE.Scene();
        const camera = new THREE.OrthographicCamera(MySetting.width / -2, MySetting.width / 2, MySetting.height / 2, MySetting.height / -2, 1, 1000);
        scene.add(camera);
        camera.position.z = 100;

        const renderer = new THREE.WebGLRenderer();
        renderer.setPixelRatio(window.devicePixelRatio);
        renderer.setSize(MySetting.width, MySetting.height, true);
        const glCanvas = document.body.appendChild(renderer.domElement);
        glCanvas.id="camera-canvas";
        gl=renderer.getContext();
        readbackFramebuffer=gl.createFramebuffer();

        const mycamera = document.getElementById('my-camera');
        const cameratex = new THREE.VideoTexture(mycamera);
        const cameramaterial = new THREE.SpriteMaterial({map:cameratex, color: 0xffffff});
        const camsprite = new THREE.Sprite(cameramaterial);
        camsprite.scale.set(MySetting.width + 10, MySetting.height + 10, 1);
        scene.add(camsprite);
        camsprite.position.set(0, 0, 5);

        const recvvideo = document.getElementById('receive-video');
        const recvvideotex = new THREE.VideoTexture(recvvideo);
        const chromamaterial = new THREE.ShaderMaterial({
            transparent: true,
            uniforms:{
                map:{value:recvvideotex},
                keyColor:{value:[0.0,1.0,0.0]},
                similarity:{value:0.68},
                smoothness:{value:0.2},
            },
            vertexShader:vertexShader(),
            fragmentShader:fragmentShader()
        })
        function vertexShader() {
            return `
                varying vec2 vUv;
                void main( void ) {     
                    vUv = uv;
                    gl_Position = projectionMatrix * modelViewMatrix * vec4(position,1.0);
                }
            `
        }
        function fragmentShader() {
            return `
                uniform vec3 keyColor;
                uniform float similarity;
                uniform float smoothness;
                varying vec2 vUv;
                uniform sampler2D map;
                void main() {
                    vec4 videoColor = texture2D(map, vUv);
                    float Y1 = 0.299 * keyColor.r + 0.587 * keyColor.g + 0.114 * keyColor.b;
                    float Cr1 = keyColor.r - Y1;
                    float Cb1 = keyColor.b - Y1;
                
                    float Y2 = 0.299 * videoColor.r + 0.587 * videoColor.g + 0.114 * videoColor.b;
                    float Cr2 = videoColor.r - Y2; 
                    float Cb2 = videoColor.b - Y2; 
                
                    float blend = smoothstep(similarity, similarity + smoothness, distance(vec2(Cr2, Cb2), vec2(Cr1, Cb1)));
                    gl_FragColor = vec4(videoColor.rgb, videoColor.a * blend); 
                }
            `
        }
        const recvsprite = new THREE.Sprite(chromamaterial);
        recvsprite.scale.set(MySetting.width +10, MySetting.height + 10, 1);
        scene.add(recvsprite);
        recvsprite.position.set(0, 0, 10);

        const animate = () => {
            renderer.render( scene, camera );
            if(MySendStatus.cstatus==-1 && !isLocalizing){ // it truly 1 !!
                localize(glCanvas);            
            }
            if(isStartSendUDP){
                // if(isStartSendUDP && sendCount>5){
                dataConnection.send(JSON.stringify(MySendStatus));
                // console.log(ret);
                // sendCount = 0;
            }
            // sendCount ++;
            window.requestAnimationFrame(animate);
        }
        window.requestAnimationFrame(animate);
        MySendStatus.cstatus=10; //complete setting
        resolve("startwebGL");
    })
}

// WEBRTC FUNCTIONS
const MakeCall = () =>{
    return new Promise((resolve, reject)=>{
        dataConnection = peer.connect(MySetting.dpeerid, {
            serialization:'none'});
        dataConnection.on("open",()=>{
            isStartSendUDP=true;
        });
        dataConnection.on("data",(data)=>{
            ReceivedStatus = JSON.parse(String.fromCharCode.apply(null, new Uint8Array(data)));
            testText.innerText = ReceivedStatus.cstatus;
    
            // console.log(ReceivedStatus);
      
        });
        mediaConnection = peer.call(MySetting.mpeerid, null, {
            videoBandWidth: 2000,
            videoCodec: 'H264',
            videoReceiveEnabled: true,
        });
        mediaConnection.on('stream', mstream => {
            console.log(mstream);
            const videoElm = document.getElementById('receive-video');

            videoElm.srcObject = mstream;
            videoElm.height=MySetting.height;
            videoElm.width=MySetting.width;
            videoElm.play();

            // initWebGL();
            resolve("getcalling");
    });
})
}


// INHERIT DEVICE SETTINGS FUNCTIONS
const cameraSetting = () =>{
    return new Promise((resolve,reject)=>{ 
    navigator.mediaDevices.getUserMedia({
        audio: false,
        video:{
            facingMode: {exact: "environment"},
            width:{min: MySetting.width},
            height:{min: MySetting.height},
            aspectRatio: MySetting.height / MySetting.width
        }
    })
    .then((stream)=>{
        const videosettings = stream.getVideoTracks()[0].getSettings();
        MySetting.videoheight=videosettings.height;
        MySetting.videowidth=videosettings.width;
        console.log([MySetting.videowidth,MySetting.videoheight]);
        console.log(stream.getVideoTracks()[0].getConstraints());

        getCameraIntrinsics(MySetting.videowidth,MySetting.videoheight);
        
        const myCameraElm=document.getElementById('my-camera');
        myCameraElm.height=MySetting.height;
        myCameraElm.width=MySetting.width;
        myCameraElm.srcObject = stream;
        myCameraElm.play();

        resolve("getcamerasettting");
        return;
    })
    .catch((err)=>{
        console.error(err);
        alert("カメラが使用できませんでした。ページを再度読み込んでください。");
        reject("cannotgetcamerasetting");
        return;
    });
})
}

// カメラパラメータを定義している。
const getCameraIntrinsics = (width,height) => {
    const fy = 960;
    cameraIntrinsics.x = fy*height/width;
    cameraIntrinsics.y = fy;
    cameraIntrinsics.z = width / 2;
    cameraIntrinsics.w = height / 2;
  }

// GET & SEND DEVICE MOTION FUNCTIONS
const handleOrientation=(e)=>{
    var ret = [0.0,0.0,0.0];
    var alpha = e.alpha;
    var beta = e.beta;
    var gamma = e.gamma;
    if(isAbsoluteSensor){
        // var rawleftpitch =  Math.asin(populateMatrix[1])/ degtorad;
        var rawleftyaw = Math.atan2(populateMatrix[2],populateMatrix[0]) / degtorad;
        // var rawleftroll = Math.atan2(-populateMatrix[9],populateMatrix[5])/ degtorad;
        
        ret[1]=-rawleftyaw;
    }else if(isIos){
        ret[1]=e.webkitCompassHeading;
    }else{
        // ret[1]=compassHeading(alpha,beta,gamma)+relativeheading;
        ret[1]=compassHeading(alpha,beta,gamma);
    }

    if(MySendStatus.cstatus==10 && relativeCount<3){
        relativeheading = ret[1] + 180;
        relativeCount++;
    }
    ret[1] = ret[1] - relativeheading;
    console.log(ret[1]);

    ret[0] = 270-(-beta || 0);

    MySendStatus.crot[0]=ret[0]+5;
    MySendStatus.crot[1]=ret[1] - 10;
    MySendStatus.crot[2]=ret[2];
    MySendStatus.csend=performance.now();
}

const compassHeading = (alpha, beta, gamma) => {  
    var _x = beta ? beta * degtorad : 0; // beta value
    var _y = gamma ? gamma * degtorad : 0; // gamma value
    var _z = alpha ? alpha * degtorad : 0; // alpha value
  
    var cX = Math.cos(_x);
    var cY = Math.cos(_y);
    var cZ = Math.cos(_z);
    var sX = Math.sin(_x);
    var sY = Math.sin(_y);
    var sZ = Math.sin(_z);
  
    // Calculate Vx and Vy components
    var Vx = -cZ * sY - sZ * sX * cY;
    var Vy = -sZ * sY + cZ * sX * cY;
  
    // Calculate compass heading
    var compassHeading = Math.atan(Vx / Vy);
  
    // Convert compass heading to use whole unit circle
    if (Vy < 0) {
      compassHeading += Math.PI;
    } else if (Vx < 0) {
      compassHeading += 2 * Math.PI;
    }
  
    return compassHeading /degtorad; // Compass Heading (in degrees)
  }

  const handleOrientation1=(e)=>{
    var retx = [0.0,0.0,0.0];
    var rety = [0.0,0.0,0.0];
    var retz = [0.0,0.0,0.0];
    if(isAbsoluteSensor){
        var ro = -(window.orientation || 0) * Math.PI / 180;
		var ry =  (e.gamma || 0) * Math.PI / 180;
		var rx =  (e.beta  || 0) * Math.PI / 180;
		var rz =  (e.alpha || 0) * Math.PI / 180;
		var co = Math.cos(ro);
		var so = Math.sin(ro);
		var cy = Math.cos(ry);
		var sy = Math.sin(ry);
		var cx = Math.cos(rx);
		var sx = Math.sin(rx);
		var cz = Math.cos(rz);
		var sz = Math.sin(rz);

		// ローカル x 軸ベクトル
		var axisX = [
			co * cy * cz - co * sy * sx * sz - so * cx * sz,
			co * cy * sz + co * sy * sx * cz + so * cx * cz,
			-co * sy * cx + so * sx
        ];
        // ローカル y 軸ベクトル
        var axisY = [
			-so * cy * cz + so * sy * sx * sz - co * cx * sz,
			-so * cy * sz - so * sy * sx * cz + co * cx * cz,
			 so * sy * cx + co * sx
        ];

		// ローカル z 軸ベクトル
		var axisZ = [
			sy * cz + cy * sx * sz,
			sy * sz - cy * sx * cz,
			cy * cx,
        ];

        retx=axisX;
        rety=axisY;
        retz=axisZ;
        }else if(isIos){
        // ret[1]=e.webkitCompassHeading;
    }else{
        // ret[1]=compassHeading(alpha,beta,gamma);
    }

    MySendStatus.crotx=retx;
    MySendStatus.croty=rety;
    MySendStatus.crotz=retz;
    MySendStatus.csend=performance.now();
    if(isStartSendUDP && sendCount>5){
    // if(isStartSendUDP){
        dataConnection.send(JSON.stringify(MySendStatus));
        console.log(MySendStatus);
        sendCount = 0;
    }
    sendCount ++;
}
var sendCount = 0;


// INITIALIZE FUNCTIONS
const checkPortrait = () =>{
    return new Promise((resolve,reject)=>{
        let angle = screen && screen.orientation && screen.orientation.angle;
        if ( angle === undefined ) {
            angle = window.orientation;    // for iOS
        }
        if(angle === 0) 
        {
            resolve("resolve");
        }else
        {
            const deviceModal = document.getElementById("device-portrait-modal");
            const orienChange=()=>{
                let angle = screen && screen.orientation && screen.orientation.angle;
                if ( angle === undefined ) {
                    angle = window.orientation;    // for iOS
                }
                if(angle === 0)
                {
                    deviceModal.classList.add("is-hidden");
                    window.removeEventListener("orientationchange", orienChange, false);
                    resolve("resolve");
                }
            }

            window.addEventListener("orientationchange", orienChange, false);
            deviceModal.classList.remove("is-hidden");
        }
    })
}

const detectIos = () => {
    const ua = navigator.userAgent.toLowerCase();
    MySetting.ua=ua;
    console.log(ua);
    return (
        ua.indexOf("iphone") >= 0 ||
        ua.indexOf("ipad") >= 0 ||
        ua.indexOf("ipod") >= 0);
};

const checkDeviceOrien = () =>{
    return new Promise((resolve, reject)=>{
        if(!detectIos())
        {// android
            isIos=false;
            const sensor = new AbsoluteOrientationSensor({frequency:60, referenceFrame:'device'});
            Promise.all([
                navigator.permissions.query({ name: "accelerometer" }),
                navigator.permissions.query({ name: "magnetometer" }),
                navigator.permissions.query({ name: "gyroscope" }),
            ]).then((results) => {
                if (results.every((result) => result.state === "granted")) {
                    testText.innerText = "Use AbsoluteOrientationSensor";
                    console.log("Use AbsoluteOrientationSensor.");
                    sensor.start();
                    isAbsoluteSensor = true;
                    sensor.addEventListener('reading', ()=>{
                        sensor.populateMatrix(populateMatrix);
                        // console.log(populateMatrix);
                    })
                    sensor.addEventListener("error", (error) => {
                        if (error.name === "NotReadableError") {
                            testText.innerText = "AbsoluteOrientSensor is not available";
                            console.log("AbsoluteOrientSensor is not available.");
                            
                        }
                        isAbsoluteSensor = false;
                        // alert("sensor is not absolute");
                      });
                    resolve("resolve");
                } else {
                    testText.innerText == "cannot use absolute sensor";
                    console.log("No permissions to use AbsoluteOrientationSensor.");
                    isAbsoluteSensor = false;
                    resolve("resolve");
                }
            });
        }else
        {//iOS
            isIos=true;
            const devicePermModal=document.getElementById("device-perm-modal");
            const devicePermModalButton=document.getElementById("device-perm-modal-button");
            const alertMessage="センサーの使用が拒否されました。\nこのページを楽しむには、デバイスモーションセンサーの使用を許可する必要があります。\nブラウザのアプリを再起動して、モーションセンサーの使用（「動作と方向」へのアクセス）を許可をしてください。";
            const hideDeviceOrienModal = () => {
                devicePermModal.classList.add("is-hidden");
            };
            const deviceOrienEvent=()=>{
                hideDeviceOrienModal();
                window.removeEventListener("deviceorientation", deviceOrienEvent, false);
                resolve("resolve");
            };
            window.addEventListener("deviceorientation", deviceOrienEvent, false);

            devicePermModal.classList.remove("is-hidden");
            //listen event click modal-button
            devicePermModalButton.addEventListener("click", () => {
              //request permission device motion
                if (
                    DeviceMotionEvent &&
                    DeviceMotionEvent.requestPermission &&
                    typeof DeviceMotionEvent.requestPermission === "function"
                ) {
                    DeviceMotionEvent.requestPermission();
                }
                if (
                    DeviceOrientationEvent &&
                    DeviceOrientationEvent.requestPermission &&
                    typeof DeviceOrientationEvent.requestPermission === "function"
                ) {
                    DeviceOrientationEvent.requestPermission().then((res) => {
                        console.log(res);
                        if (res === "granted") {
                            testText.innerText = "iOS webkit compass";
                            hideDeviceOrienModal();
                            resolve("resolve");
                        } else {
                            alert(alertMessage);
                            reject("reject");
                        }
                    });
                } else {
                    alert(alertMessage);
                    reject("reject");
                }
            });
        }
    });
};

const checkDeviceLocation = () => {
    return new Promise((resolve, reject)=>{
        if(!navigator.geolocation) 
        {
            alert("位置情報が取得できませんでした。\n端末のGPS設定をONにして、再度ページを読み込んでください。");
            reject("reject");
        }

        const deviceLocaModal=document.getElementById("device-loca-modal");
        const hideDeviceLocaModal = () => {
            deviceLocaModal.classList.add("is-hidden");
        };
        const success = (pos) => {
            const crd=pos.coords;
            if( // サービスを利用できる座標を制限していた。
                // 10km square
                // crd.latitude>35.5345 &&
                // crd.latitude<35.6245 &&
                // crd.longitude>135.124 &&
                // crd.longitude<135.234
                true
            )
            {
                hideDeviceLocaModal();
                resolve("resolve");        
            }
            else
            {
                alert("エリア外です。\n本コンテンツは丹後五重塔跡地周辺でのみお楽しみいただけます。");
                reject("reject");
            }
        };
        const error = (err) => {
            console.warn(`ERROR(${err.code}): ${err.message}`);
            alert("位置情報が取得できませんでした。\n端末のGPS設定をONにして、再度ページを読み込んでください。");
            reject("reject");
        };
        const opt={
            enableHighAccuracy:true,
            timeout:10000,
            maximumAge:0
        };

        deviceLocaModal.classList.remove("is-hidden");
        navigator.geolocation.getCurrentPosition(success,error,opt);
    })
}

const showForm = () => {
    // コンテンツ終了後、ユーザ識別子を事前入力した状態のGoogleFormに飛べるようにしていた。
    const formURL = "GoogleForm_URL" + MySendStatus.client;
    const formDiv = document.getElementById("form-div");
    const formLink = document.getElementById("form-link");
    document.body.appendChild(formDiv);
    formLink.onclick = ()=>{
        wantquit();
        location.replace(formURL);
        return false;
    }
    formDiv.classList.add('fadein');
    formDiv.classList.remove('is-hidden');
}

// 夜コンテンツは動画からMRへシームレスに転換させていた。
// let loadedVideo = false;
let loadedAudio = false;
const gettouch=()=>{
    const startModal = document.getElementById("device-start-modal");
    startModal.classList.add('is-hidden');
    // const videoElm=document.getElementById('pre-video');
    const audioElm=document.getElementById('bg-music');
    // videoElm.load();
    // videoElm.addEventListener("canplay",()=>{
    //     loadedVideo=true;
    // });
    audioElm.load();
    audioElm.addEventListener("canplay",()=>{
        loadedAudio=true;
    });
}
const loadstart=()=>{
    return new Promise((resolve,reject)=>{
        document.body.addEventListener("touchstart",gettouch,false);
        document.body.addEventListener("touchstart",()=>{
             resolve("startload");
        })
    })
}
const touchStartModal = () =>{
        const deviceModal = document.getElementById("device-touch-modal");
        deviceModal.classList.remove('is-hidden');
        const cameracanvas = document.getElementById("camera-canvas");
        cameracanvas.classList.add('looks-hidden');
        // const videoElm=document.getElementById('pre-video');
        // videoElm.classList.remove('looks-hidden');
        const audioElm=document.getElementById('bg-music');

        const intraImg = document.getElementById('intra-img');

        const setTimers = () =>{
            document.body.removeEventListener('touchstart',onStage);
            setTimeout(()=>{
                cameracanvas.classList.remove('looks-hidden');
                // videoElm.classList.add('fadeout');
                // },11500);
                },500);
            setTimeout(()=>{
                MySendStatus.cstatus = 10;
                // },12000);
                },1000);
            setTimeout(()=>{
                document.body.appendChild(intraImg);
                intraImg.classList.remove('is-hidden');
                setTimeout(()=>{
                    intraImg.classList.add('is-hidden');
                    },48500);
                },101000);
            setTimeout(showForm,195000);
        }
        const onStage = () =>{
            deviceModal.classList.add('is-hidden');
            console.log("onStage!!");
            
            // videoElm.play();
            audioElm.play();
            setTimers();
        }
        const onReady = setInterval(()=>{
            // if(loadedVideo && loadedAudio){
            if(loadedAudio){
                deviceModal.getElementsByTagName('p')[0].innerText="画面をタッチしてARを開始します。";
                document.body.removeEventListener("touchstart",gettouch);
                document.body.addEventListener('touchstart', onStage,false);
                clearInterval(onReady);
                loadedData=0;
            }
        },100);
        onReady();
}

const showFormDay = () => {
    const formURL = "GoogleForm_URL" + MySendStatus.client;
    const underForm = document.getElementById("under-form");
    const underFormLink = document.getElementById("under-form-link");
    const formDiv = document.getElementById("form-div");
    const formLink = document.getElementById("form-link");
    
    document.body.appendChild(underForm);
    underForm.classList.remove("is-hidden");
    underFormLink.onclick = ()=>{
        wantquit();
        location.replace(formURL);
        return false;
    }

    setTimeout(()=>{
        underForm.classList.add("is-hidden");
        document.body.appendChild(formDiv);
        formDiv.classList.add('fadein');
        formDiv.classList.remove('is-hidden');
        formLink.onclick = ()=>{
            wantquit();
            location.replace(formURL);
            return false;
        }
    },180000)
}

const touchStartModalDay = () =>{
    const sessyuBg = document.getElementById("sessyu-bg");
    sessyuBg.classList.remove('is-hidden');
    const cameracanvas = document.getElementById("camera-canvas");
    cameracanvas.classList.add('looks-hidden');

    const setTimers = () =>{
        setTimeout(()=>{
            cameracanvas.classList.remove('looks-hidden');
            cameracanvas.classList.add('fadein');
        },500);
        setTimeout(showFormDay,10500);
    }
    MySendStatus.cstatus = 10;
    setTimers();
}

//　デバイスの時計を元に昼と夜でコンテンツを切り替えていた。
//WINDOW ONLOAD
const now = new Date();
// if(now.getHours() >= 17 || 6 > now.getHours()){ //nighttime
if(true){ //nighttime
MySendStatus.cloc =[2400.0, -900.0, 900.0];
window.onload = () => {
    window.addEventListener("popstate", function (e) {  // prohibit browser back
        history.pushState(null, null, null);
        return;
    });
    document.getElementById("device-start-modal").classList.remove('is-hidden');
    loadstart()
    .then(()=>{
    var testText = document.getElementById("testText");
    testText.innerText = "nighttime";
    //firstTouch().then(()=>{
        checkPortrait()
        .then(()=>{
            checkDeviceOrien()
            .then(() => {
            //   checkDeviceLocation()
            //   .then(()=>{
                MySendStatus.cstatus=1;

                window.addEventListener("deviceorientation", handleOrientation,false);//start get device rotation
                                
                    RequestMakeClient()
                    .then((ret)=>{
                        console.log(ret);
                        getCallee()
                        .then((ret)=>{
                            console.log(ret);
                            window.addEventListener('beforeunload', wantquit, false);
                            cameraSetting()
                            .then((ret)=>{
                                console.log(ret);
                                MakeCall()
                                .then((ret)=>{
                                    console.log(ret);
                                    initWebGL()
                                    .then((ret)=>{
                                        console.log(ret);
                                        touchStartModal();
                                    })
                                    .catch(()=>{
                                        console.warn("webGLerror");    
                                    })
                                })
                                .catch(()=>{
                                    console.warn("errorwithgetcall");
                                })
                            })
                            .catch((error)=>{
                                console.warn(error);
                            })
                        })
                        .catch((error)=>{
                            console.warn(error);
                        })
                    })
                    .catch((error)=>{
                        console.warn(error);//can not make client
                    })
                
            // })

            // .catch((error)=>{
            //     console.warn(error);//can NOT use GPS or out of area
            // })
        })
        .catch((error) => {
          console.warn(error);//can NOT use device motion
        })
    })
})
};

} else { //daytime
    MySendStatus.cloc =[3200.0, -1200.0, 900.0];
    window.onload = () => {
        window.addEventListener("popstate", function (e) {  // prohibit browser back
            history.pushState(null, null, null);
            return;
        });
        var testText = document.getElementById("testText");
        testText.innerText = "daytime";
        //firstTouch().then(()=>{
            checkPortrait()
            .then(()=>{
                checkDeviceOrien()
                .then(() => {
                  checkDeviceLocation()
                  .then(()=>{
                    MySendStatus.cstatus=1;
    
                    window.addEventListener("deviceorientation", handleOrientation,false);//start get device rotation
                                    
                        RequestMakeClient()
                        .then((ret)=>{
                            console.log(ret);
                            getCallee()
                            .then((ret)=>{
                                console.log(ret);
                                window.addEventListener('beforeunload', wantquit, false);
                                cameraSetting()
                                .then((ret)=>{
                                    console.log(ret);
                                    MakeCall()
                                    .then((ret)=>{
                                        console.log(ret);
                                        initWebGLDay()
                                        .then((ret)=>{
                                            console.log(ret);
                                            touchStartModalDay();
                                        })
                                        .catch(()=>{
                                            console.warn("webGLerror");    
                                        })
                                    })
                                    .catch(()=>{
                                        console.warn("errorwithgetcall");
                                    })
                                })
                                .catch((error)=>{
                                    console.warn(error);
                                })
                            })
                            .catch((error)=>{
                                console.warn(error);
                            })
                        })
                        .catch((error)=>{
                            console.warn(error);//can not make client
                        })
                    
                })
    
                .catch((error)=>{
                    console.warn(error);//can NOT use GPS or out of area
                })
            })
            .catch((error) => {
              console.warn(error);//can NOT use device motion
            })
        })
    };
}