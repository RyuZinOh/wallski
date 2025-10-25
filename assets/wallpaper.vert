attribute vec2 aPos;
attribute vec2 aTex;
varying vec2 vTex;

void main(){
  vTex = aTex;
  gl_Position = vec4(aPos, 0.0, 1.0);
}
