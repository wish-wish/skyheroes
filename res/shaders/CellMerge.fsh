
#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform sampler2D u_texture1;
uniform float u_interpolate;
uniform int   u_type;
uniform vec4  u_src_color;
uniform vec4  u_dst_color;

void main() {
	if(u_type==0)
	{
		vec4 color1 = texture2D(CC_Texture0, v_texCoord);
    	vec4 color2 = texture2D(u_texture1, v_texCoord);
    	gl_FragColor = v_fragmentColor * mix( color1, color2, u_interpolate);
	}
	else if(u_type==1)
	{
		vec4 color1 = texture2D(CC_Texture0, v_texCoord);
    	vec4 color2 = u_src_color;
    	gl_FragColor = v_fragmentColor * mix( color1, color2, u_interpolate);
	}
	else if(u_type==2)
	{
		vec4 color1 = u_dst_color;
    	vec4 color2 = texture2D(CC_Texture0, v_texCoord);
    	gl_FragColor = v_fragmentColor * mix( color1, color2, u_interpolate);
	}
	else if(u_type==3)
	{
		vec4 color1 = u_dst_color;
    	vec4 color2 = u_src_color;
    	gl_FragColor = v_fragmentColor * mix( color1, color2, u_interpolate);
	}
}

