!!NVfp4.0
TEMP Z;
CENTROID ATTRIB texcoord = fragment.texcoord[0];
TEX Z, texcoord, texture[0], 2D;
MUL result.color, Z, fragment.color;
END
