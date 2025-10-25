precision mediump float;
varying vec2 vTex;
uniform sampler2D tex;
void  main(){
  gl_FragColor = texture2D(tex, vTex);
}

