
in vec4 vert_pos;

uniform sampler2D texture;
uniform bool hasTexture;
uniform vec2 lightPos;

void main()
{
	//Ambient light based on 1, (1 is full light 0 is no light. .5 is half etc.)
	vec4 ambient = vec4(.52, 0.52, 0.5, 1.0);
	
	//Convert light to view coords
	vec2 lightPos1 = (gl_ModelViewProjectionMatrix * vec4(lightPos, 0, 1)).xy;
	
	//Calculate the vector from light to pixel (Make circular)
	vec2 lightToFrag = lightPos1 - vert_pos.xy;
	lightToFrag.y = lightToFrag.y / 1.7;

	//Length of the circle (distance)
	float vecLength = clamp(length(lightToFrag) * 2, 0, .7);

    // lookup the pixel in the texture
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

    // multiply it by the color and lighting
	if(hasTexture == true)
	{
		gl_FragColor = gl_Color * pixel * (clamp(ambient + vec4(1-vecLength, 1-vecLength, 1-vecLength, 1), 0, 1));
	}
	else
	{
		gl_FragColor = gl_Color;
	}
}