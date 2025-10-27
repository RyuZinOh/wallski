precision mediump float;
varying vec2 vTex;
uniform sampler2D tex_old;
uniform sampler2D tex_new;
uniform float u_progress;
//0=wipe, 1 =shatter, 2=fade
uniform int u_type; 

void  main(){
  vec4 oldColor = texture2D(tex_old, vTex);
  vec4 newColor = texture2D(tex_new, vTex);

  if(u_progress>=0.99){
    gl_FragColor  = newColor;
    return;
  }
  float factor = 0.0;
  if(u_type == 0){
    factor =  smoothstep(0.0,0.005, vTex.x-(1.0-u_progress));
  }else if(u_type ==1){
    float block = fract(vTex.x*10.0+vTex.y*10.0);
    factor =  step(1.0-u_progress,block );
  }else if(u_type==2){
    factor = u_progress;
  }
  gl_FragColor = mix(oldColor, newColor, factor );
}

