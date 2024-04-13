#define PROGMEM

const char index_html[] PROGMEM = R"rawliteral(<!DOCTYPE HTML>
<html lang='ua'>
<head>
<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'/>
<meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1.0 user-scalable=no'/>
<meta http-equiv='X-UA-Compatible' content='IE=edge'/>
<meta name='msapplication-tap-highlight' content='no'/>
<meta name='msapplication-TileColor' content='#455a64'/>
<meta name='theme-color' content='#455a64'/>
<title>Test</title>
<link rel='icon' href='data:,'/>
</head>

<body>
<center>
<div id="rng">%rng1%, %rng2%, %rng3%</div>
<p></p>
<div><a href='plot'>Plot 2D</a></div>
<p></p>
<div><a href='cosmos'>Cosmos 3D</a></div>
<p></p>
<div><a href='chess'>Chess</a></div>
<p></p>
<div>%connect%</div>
</center>
<script>
var xhttp = new XMLHttpRequest();

xhttp.onreadystatechange = function () {
  if (this.readyState == 4 && this.status == 200) {
    var str = this.responseText;      //  0    1    2
    var arr = str.split('/');         // "rng1/rng2/rng3"
    if (arr.length == 3) {
      document.getElementById("rng").innerHTML = arr[0]+", "+arr[1]+", "+arr[2];
    }
  }
}

setInterval(function(){xhttp.open("GET", "/rng", true); xhttp.send();}, 10);
</script>
</body>
</html>)rawliteral";

const char plot_html[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang='en'>
<head>
<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'/>
<meta http-equiv='X-UA-Compatible' content='IE=edge'/>
<meta name='msapplication-tap-highlight' content='no'/>
<meta name='msapplication-TileColor' content='#455a64'/>
<meta name='theme-color' content='#455a64'/>
<title>Plot 2D</title>
<link rel='icon' href='data:,'/>
<script src='https://cdn.plot.ly/plotly-2.30.0.min.js'></script>
</head>

<body>
<div id='myDiv'></div>
<script>
var arr_x = [%arr_x%];
var arr_y = [%arr_y%];
Plotly.newPlot('myDiv', [{x: arr_x, y: arr_y, mode: 'markers'}], {height: 900, width: 1900});
</script>
</body>
</html>)rawliteral";

const char cosmos_html[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, user-scalable=no, minimum-scale=1.0, maximum-scale=1.0">
<meta http-equiv='X-UA-Compatible' content='IE=edge'/>
<meta name='msapplication-tap-highlight' content='no'/>
<meta name='msapplication-TileColor' content='#455a64'/>
<meta name='theme-color' content='#455a64'/>
<title>Cosmos 3D</title>
<link rel='icon' href='data:,'/>
</head>

<body style="margin: 0; background-color: #000000">
<script src="https://zxlogin.com/3d/cosmos/js/Three.js"></script>
<script src="https://zxlogin.com/3d/cosmos/js/Detector.js"></script>
<script src="https://zxlogin.com/3d/cosmos/js/Stats.js"></script>
<script src="https://zxlogin.com/3d/cosmos/js/OrbitControls.js"></script>
<script src="https://zxlogin.com/3d/cosmos/js/KeyboardState.js"></script>
<script src="https://zxlogin.com/3d/cosmos/js/THREEx.FullScreen.js"></script>
<script src="https://zxlogin.com/3d/cosmos/js/THREEx.WindowResize.js"></script>
<div id="ThreeJS" style="position: absolute; left: 0px; top: 0px"></div>
<script>
var container, scene, camera, renderer, controls, z = 0;
var keyboard = new KeyboardState();
var clock = new THREE.Clock();

init();
animate();

function init() {
	// SCENE
	scene = new THREE.Scene();
	
	// CAMERA
	var SCREEN_WIDTH  = window.innerWidth;
	var SCREEN_HEIGHT = window.innerHeight;
	var VIEW_ANGLE = 45, ASPECT = SCREEN_WIDTH/SCREEN_HEIGHT, NEAR = 0.1, FAR = 10000;
	
	camera = new THREE.PerspectiveCamera(VIEW_ANGLE, ASPECT, NEAR, FAR);
	scene.add(camera);
	camera.position.set(0, 150, 400);
	camera.lookAt(scene.position);	
	
	// RENDERER
	if (Detector.webgl) {renderer = new THREE.WebGLRenderer({antialias: true});}
	else {renderer = new THREE.CanvasRenderer();}
	renderer.setSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	
	container = document.getElementById('ThreeJS');
	container.appendChild(renderer.domElement);
	
	// EVENTS
	THREEx.WindowResize(renderer, camera);
	THREEx.FullScreen.bindKey({charCode: 'm'.charCodeAt(0)});
	
	// CONTROLS
	controls = new THREE.OrbitControls(camera, renderer.domElement);
	
	var geometry2 = new THREE.SphereGeometry(1, 16, 16);
	var material2 = new THREE.MeshNormalMaterial();
	var sphere = new THREE.Mesh(geometry2, material2);

	// 6318161
	for (var i=0; i<50000; i++) {
		let x = randomInteger(-1000, 1000);
		let y = randomInteger(-1000, 1000);
		let z = randomInteger(-1000, 1000);
		var sphere = new THREE.Mesh(geometry2, material2);
		sphere.position.set(x, y, z);
		scene.add(sphere);
	}
}

function animate() {
	//if (keyboard.pressed("z")) {z = 1;}
	//if (z == 1) {camera.position.z -= 1;}
	camera.position.z -= 1;
	requestAnimationFrame(animate);
	renderer.render(scene, camera);
	controls.update();
}

function randomInteger(min, max) {return Math.floor(min+Math.random()*(max+1-min));}
</script>
</body>
</html>)rawliteral";

const char chess_html[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang='en'>
<head>
<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'/>
<meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1.0 user-scalable=no'/>
<meta http-equiv='X-UA-Compatible' content='IE=edge'/>
<meta name='msapplication-tap-highlight' content='no'/>
<meta name='msapplication-TileColor' content='#455a64'/>
<meta name='theme-color' content='#455a64'/>
<title>Chess</title>
<link rel='icon' href='data:,'/>
<style>body {background-color:#455a64;}</style>
</head>

<body>
<script>
var B,i,y,u,b,I=[],G=120,x=10,z=15,M=1e4,l=[5,3,4,6,2,4,3,5,1,1,1,1,1,1,1,1,9,9
,9,9,9,9,9,9,13,11,12,14,10,12,11,13,0,99,0,306,297,495,846,-1,0,1,2,2,1,0,-1,-
1,1,-10,10,-11,-9,9,11,10,20,-9,-11,-10,-20,-21,-19,-12,-8,8,12,19,21];function
X(w,c,h,e,S,s){var t,o,L,E,d,O=e,N=-M*M,K=78-h<<x,p,g,n,m,A,q,r,C,J,a=y?-x:x;
y^=8;G++;d=w||s&&s>=h&&X(0,0,0,21,0,0)>M;do{if(o=I[p=O]){q=o&z^y;if(q<7){A=q--&
2?8:4;C=o-9&z?[53,47,61,51,47,47][q]:57;do{r=I[p+=l[C]];if(!w|p==w){g=q|p+a-S?0
:S;if(!r&(!!q|A<3||!!g)||(r+1&z^y)>9&&q|A>2){if(m=!(r-2&7))return y^=8,I[G--]=
O,K;J=n=o&z;E=I[p-a]&z;t=q|E-7?n:(n+=2,6^y);while(n<=t){L=r?l[r&7|32]-h-q:0;if(
s)L+=(1-q?l[(p-p%x)/x+37]-l[(O-O%x)/x+37]+l[p%x+38]*(q?1:2)-l[O%x+38]+(o&16)/2:
!!m*9)+(!q?!(I[p-1]^n)+!(I[p+1]^n)+l[n&7|32]-99+!!g*99+(A<2):0)+!(E^y^9);if(s>h
||1<s&s==h&&L>z|d){I[p]=n,I[O]=m?(I[g]=I[m],I[m]=0):g?I[g]=0:0;L-=X(s>h|d?0:p,L
-N,h+1,I[G+1],J=q|A>1?0:p,s);if(!(h||s-1|B-O|i-n|p-b|L<-M))return W(),G--,u=J;
J=q-1|A<7||m||!s|d|r|o<z||X(0,0,0,21,0,0)>M;I[O]=o;I[p]=r;m?(I[m]=I[g],I[g]=0):
g?I[g]=9^y:0;}if(L>N||s>1&&L==N&&!h&&Math.random()<.5){I[G]=O;if(s>1){if(h&&c-L
<0)return y^=8,G--,L;if(!h)i=n,B=O,b=p;}N=L;}n+=J||(g=p,m=p<O?g-3:g+2,I[m]<z|I[
m+O-p]||I[p+=p-O])?1:0;}}}}while(!r&q>2||(p=O,q|A>2|o>z&!r&&++C*--A));}}}while(
++O>98?O=20:e-O);return y^=8,G--,N+M*M&&N>-K+1924|d?N:0;}B=i=y=u=0;while(B++<
120)I[B-1]=B%x?B/x%x<2|B%x<2?7:B/x&4?0:l[i++]|16:7;for(a=
'<table cellspacing=0 align=center>',i=18;i<100;a+=++i%10-9?
'<th width=60 height=60 onclick=Y('+i+') id=o'+i+
' style=\"line-height:50px;font-size:50px;border:2px solid #dde\" bgcolor=#'+
(i*.9&1?'c0c':'f0f')+'0f0>':(i++,'<tr>'));
a+='<th colspan=8><select id=t style=\"font-size:20px\"><option>&#9819;<option>';
document.write(a+'&#9820;<option>&#9821;<option>&#9822;</select></table>');
function W(){B=b;for(p=21;p<99;++p)if(q=document.getElementById('o'+p)){q.
innerHTML='\x20\u265f\u265a\u265e\u265d\u265c\u265b  \u2659\u2654\u2658\u2657\u2656\u2655'.charAt(I[p]&z);
q.style.borderColor=p==B?'red':'#dde';}}W();
function Y(s){i=(I[s]^y)&z;if(i>8){b=s;W();}else if(B&&i<9){b=s;i=I[B]&z;if((i&
7)==1&(b<29|b>90))i=14-document.getElementById('t').selectedIndex^y;X(0,0,0,21,
u,1);if(y)setTimeout('X(0,0,0,21,u,2/*ply*/),X(0,0,0,21,u,1)',250);}}
</script>
</body>
</html>)rawliteral";

const char p404_html[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html>
<head>
<title>404</title>
<meta charset="utf-8"/>
<meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1.0 user-scalable=no'/>
<link rel='icon' href='data:,'/>
<style>
body {font-family: 'Source Sans Pro', sans-serif; background-color: #323133;}
body:after {content: ''; position: absolute; top: 0px; left: 0px; right: 0px; bottom: 0px; z-index: 1000;
  background: repeating-linear-gradient(0deg, #111 0px, #111 1px, transparent 2px, transparent 5px);
  background-size: 100% 5px; opacity: 0.3; mix-blend-mode: color-burn; pointer-events: none;
  -webkit-animation: lines 2s ease-out infinite;
          animation: lines 2s ease-out infinite;
}
body .text, body .text > * {position: absolute; top: 50%; left: 50%;
  -webkit-transform: translate(-50%, -50%);
          transform: translate(-50%, -50%);
}
body .text > * {font-size: 40px; mix-blend-mode: difference;
  -webkit-animation: giggle 1s ease infinite;
          animation: giggle 1s ease infinite;
}
body .text .r {color: #f00; left: -0.5px; font-size: calc(4vw + 4vh + 2vmin);}
body .text .g {color: #0f0; font-size: calc(4vw + 4vh + 2vmin);
  -webkit-animation-delay: -0.67s;
          animation-delay: -0.67s;
}
body .text .b {color: #00f; left: 0.5px; font-size: calc(4vw + 4vh + 2vmin);
  -webkit-animation-delay: -0.33s;
          animation-delay: -0.33s;
}
@-webkit-keyframes giggle {
  0%, 100% {
    -webkit-transform: translate(-50%, -50%) translateY(-2px);
            transform: translate(-50%, -50%) translateY(-2px);
  }
  50% {
    -webkit-transform: translate(-50%, -50%) translateY(2px);
            transform: translate(-50%, -50%) translateY(2px);
  }
}
@keyframes giggle {
  0%, 100% {
    -webkit-transform: translate(-50%, -50%) translateY(-2px);
            transform: translate(-50%, -50%) translateY(-2px);
  }
  50% {
    -webkit-transform: translate(-50%, -50%) translateY(2px);
            transform: translate(-50%, -50%) translateY(2px);
  }
}
@-webkit-keyframes lines {
  0% {background-position: 0px 0px;}
  100% {background-position: 0px 25px;}
}
@keyframes lines {
  0% {background-position: 0px 0px;}
  100% {background-position: 0px 25px;}
}
</style>
</head>

<body>
<div class="text">
<div class="r">404 Nothing interesting here<br/>¯\_(ツ)_/¯</div>
<div class="g">404 Nothing interesting here<br/>¯\_(ツ)_/¯</div>
<div class="b">404 Nothing interesting here<br/>¯\_(ツ)_/¯</div>
</div>
</body>
</html>)rawliteral";
