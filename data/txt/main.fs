!!NVfp4.0
TEMP Z;
TXB Z, fragment.texcoord[0], texture[0], 2D;
MUL result.color, Z, fragment.color;
END
