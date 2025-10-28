attribute vec2 apos;
attribute vec2 atex;
varying vec2 vTex;

uniform float u_cursor;
uniform float u_img_width;
uniform float u_view_width;

void main(){
  float max_offset = (u_img_width - u_view_width)/u_img_width;
  float offset = u_cursor*max_offset;
  vTex = vec2(atex.x*(u_view_width/u_img_width)+offset,atex.y);
  gl_Position = vec4(apos, 0.0, 1.0);
}
