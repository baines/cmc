!!NVvp4.0

UINT TEMP Tx;
FLOAT TEMP Fl;

ATTRIB pos = vertex.attrib[0];
ATTRIB col = vertex.attrib[3];
UINT ATTRIB txc = vertex.attrib[8];

# calculate camera
DP4 result.position.x, state.matrix.mvp.row[0], pos;
DP4 result.position.y, state.matrix.mvp.row[1], pos;
DP4 result.position.z, state.matrix.mvp.row[2], pos;
DP4 result.position.w, state.matrix.mvp.row[3], pos;
MOV result.color, col;

# use the vertex ID % 4 to get the texcoord adjustment
AND.U Tx.x, vertex.id, 3;

AND.U Tx.z, Tx.x, 1;
SHR.U Tx.w, Tx.x, 1;
XOR.U Tx.z, Tx.z, Tx.w;

# unpack the x and y texcoords from the single byte input
AND.U Tx.x, txc.x, 0x0F;
SHR.U Tx.y, txc.x, 4;

# add the adjustment to the texcoords
ADD.U Tx.xy, Tx.xyxy, Tx.zwzw;

# divide them by 16
I2F.U Fl.xy, Tx.xyxy;
MUL.F result.texcoord[0].xy, Fl.xyxy, 0.0625;
END

