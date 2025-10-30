precision mediump float;
varying vec2 vTex;
uniform sampler2D tex_old;
uniform sampler2D tex_new;
uniform float u_progress;
//0=wipe, 1 =shatter, 2=fade 3=ripple, 4=zoom
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
    float block = fract(vTex.x * 10.0 + vTex.y * 10.0 + fract(sin(dot(vTex*100.0, vec2(12.9898,78.233))) * 43758.5453));
    factor =  step(1.0-u_progress,block );
  }else if(u_type==2){
    factor = u_progress;
  }else if(u_type==3){
    vec2 center  = vec2(0.5,0.5);
    float dist = distance(vTex,center );

    float radius = pow(u_progress,0.7)*1.5;
    float freq = 18.0;
    float amp = 0.02;

    float wave = sin((dist - radius)*freq) *amp *(1.0-u_progress);
    vec2 dir = normalize(vTex- center);
    if(dist <0.0001){
      dir = vec2(0.0);
    }
    
    vec2 rippleUV = mix(vTex, vTex+dir*wave,0.5);

    vec4 oldRipple = texture2D(tex_old, rippleUV);
    vec4 newRipple = texture2D(tex_new, rippleUV);
    
    float rippleBlend = smoothstep(radius-0.15, radius+0.15,dist);
    factor = smoothstep(0.0,1.0 ,pow(u_progress, 1.5) );
    gl_FragColor = mix(oldRipple,newRipple ,rippleBlend*factor );
    return;
  }else if(u_type == 4){
    vec2 c = vec2(0.5);
    float ep = u_progress * u_progress * (3.0 - 2.0 * u_progress);
    float oldScale = 1.0 + ep * 0.5;
    float newScale = 1.5 - ep * 0.5;

    vec2 oldUV = c + (vTex - c) * oldScale;
    vec2 newUV = c + (vTex - c) * newScale;

    vec4 o = texture2D(tex_old, oldUV);
    vec4 n = texture2D(tex_new, newUV);

    float oa = step(0.0, oldUV.x) * step(oldUV.x, 1.0) * step(0.0, oldUV.y) * step(oldUV.y, 1.0);
    float na = step(0.0, newUV.x) * step(newUV.x, 1.0) * step(0.0, newUV.y) * step(newUV.y, 1.0);

    vec4 color = mix(o * oa, n * na, step(0.5, ep));
    gl_FragColor = color;
    return;
}

  gl_FragColor = mix(oldColor, newColor, factor );
}

