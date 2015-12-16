@REM texconv is available from https://github.com/Microsoft/DirectXTex

texconv -sx _DXT1           -f BC1_UNORM            x.tif
texconv -sx _DXT2           -f BC2_UNORM -pmalpha   x.tif
texconv -sx _DXT3           -f BC2_UNORM            x.tif
texconv -sx _DXT4           -f BC3_UNORM -pmalpha   x.tif
texconv -sx _DXT5           -f BC3_UNORM            x.tif
texconv -sx _BC1_UNORM      -f BC1_UNORM      -dx10 x.tif
texconv -sx _BC2_UNORM      -f BC2_UNORM      -dx10 x.tif
texconv -sx _BC3_UNORM      -f BC3_UNORM      -dx10 x.tif
texconv -sx _BC1_UNORM_SRGB -f BC1_UNORM_SRGB -dx10 x.tif
texconv -sx _BC2_UNORM_SRGB -f BC2_UNORM_SRGB -dx10 x.tif
texconv -sx _BC3_UNORM_SRGB -f BC3_UNORM_SRGB -dx10 x.tif
texconv -sx _D3DFMT_ARGB    -f B8G8R8A8_UNORM       x.tif
texconv -sx _R8G8B8A8_UNORM -f R8G8B8A8_UNORM -dx10 x.tif
texconv -sx _B8G8R8A8_UNORM -f B8G8R8A8_UNORM       x.tif
texconv -sx _1x1            -f BC1_UNORM -w 1 -h 1  x.tif
texconv -sx _2x2            -f BC1_UNORM -w 2 -h 2  x.tif
texconv -sx _3x3            -f BC1_UNORM -w 3 -h 3  x.tif
texconv -sx _4x4            -f BC1_UNORM -w 4 -h 4  x.tif
texconv -sx _4x5            -f BC1_UNORM -w 4 -h 5  x.tif
texconv -sx _5x4            -f BC1_UNORM -w 5 -h 4  x.tif

