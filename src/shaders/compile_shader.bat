REM Regular rectangle
glslangValidator --target-env vulkan1.0 -S vert --vn ShaderRect_VS -o Rect_VS.h glsl/Rect_VS.glsl
glslangValidator --target-env vulkan1.0 -S geom --vn ShaderRect_GS -o Rect_GS.h glsl/Rect_GS.glsl
glslangValidator --target-env vulkan1.0 -S frag --vn ShaderRect_FS -o Rect_FS.h glsl/Rect_FS.glsl

glslangValidator --target-env vulkan1.0 -S geom -DKUAS_ANTIALIAS --vn ShaderRectAA_GS -o RectAA_GS.h glsl/Rect_GS.glsl
glslangValidator --target-env vulkan1.0 -S frag -DKUAS_ANTIALIAS --vn ShaderRectAA_FS -o RectAA_FS.h glsl/Rect_FS.glsl

REM Rounded rectangle
glslangValidator --target-env vulkan1.0 -S vert -DKUAS_ROUNDED_SHAPE --vn ShaderRoundedRect_VS -o RoundedRect_VS.h glsl/Rect_VS.glsl
glslangValidator --target-env vulkan1.0 -S geom -DKUAS_ROUNDED_SHAPE --vn ShaderRoundedRect_GS -o RoundedRect_GS.h glsl/Rect_GS.glsl
glslangValidator --target-env vulkan1.0 -S frag -DKUAS_ROUNDED_SHAPE --vn ShaderRoundedRect_FS -o RoundedRect_FS.h glsl/Rect_FS.glsl

glslangValidator --target-env vulkan1.0 -S geom -DKUAS_ANTIALIAS -DKUAS_ROUNDED_SHAPE --vn ShaderRoundedRectAA_GS -o RoundedRectAA_GS.h glsl/Rect_GS.glsl
glslangValidator --target-env vulkan1.0 -S frag -DKUAS_ANTIALIAS -DKUAS_ROUNDED_SHAPE --vn ShaderRoundedRectAA_FS -o RoundedRectAA_FS.h glsl/Rect_FS.glsl

REM Regular filled rectangle
glslangValidator --target-env vulkan1.0 -S vert -DKUAS_FILL_SHAPE --vn ShaderFillRect_VS -o FillRect_VS.h glsl/Rect_VS.glsl
glslangValidator --target-env vulkan1.0 -S geom -DKUAS_FILL_SHAPE --vn ShaderFillRect_GS -o FillRect_GS.h glsl/Rect_GS.glsl
glslangValidator --target-env vulkan1.0 -S frag -DKUAS_FILL_SHAPE --vn ShaderFillRect_FS -o FillRect_FS.h glsl/Rect_FS.glsl

glslangValidator --target-env vulkan1.0 -S geom -DKUAS_ANTIALIAS -DKUAS_FILL_SHAPE --vn ShaderFillRectAA_GS -o FillRectAA_GS.h glsl/Rect_GS.glsl
glslangValidator --target-env vulkan1.0 -S frag -DKUAS_ANTIALIAS -DKUAS_FILL_SHAPE --vn ShaderFillRectAA_FS -o FillRectAA_FS.h glsl/Rect_FS.glsl

REM Fill rounded rectangle
glslangValidator --target-env vulkan1.0 -S vert -DKUAS_FILL_SHAPE -DKUAS_ROUNDED_SHAPE --vn ShaderFillRoundedRect_VS -o FillRoundedRect_VS.h glsl/Rect_VS.glsl
glslangValidator --target-env vulkan1.0 -S geom -DKUAS_FILL_SHAPE -DKUAS_ROUNDED_SHAPE --vn ShaderFillRoundedRect_GS -o FillRoundedRect_GS.h glsl/Rect_GS.glsl
glslangValidator --target-env vulkan1.0 -S frag -DKUAS_FILL_SHAPE -DKUAS_ROUNDED_SHAPE --vn ShaderFillRoundedRect_FS -o FillRoundedRect_FS.h glsl/Rect_FS.glsl

glslangValidator --target-env vulkan1.0 -S geom -DKUAS_ANTIALIAS -DKUAS_FILL_SHAPE -DKUAS_ROUNDED_SHAPE --vn ShaderFillRoundedRectAA_GS -o FillRoundedRectAA_GS.h glsl/Rect_GS.glsl
glslangValidator --target-env vulkan1.0 -S frag -DKUAS_ANTIALIAS -DKUAS_FILL_SHAPE -DKUAS_ROUNDED_SHAPE --vn ShaderFillRoundedRectAA_FS -o FillRoundedRectAA_FS.h glsl/Rect_FS.glsl