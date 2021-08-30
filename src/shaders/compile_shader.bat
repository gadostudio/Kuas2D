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

REM Circle
glslangValidator --target-env vulkan1.0 -S vert --vn ShaderCircle_VS -o Circle_VS.h glsl/Circle_VS.glsl
glslangValidator --target-env vulkan1.0 -S geom --vn ShaderCircle_GS -o Circle_GS.h glsl/Circle_GS.glsl
glslangValidator --target-env vulkan1.0 -S frag --vn ShaderCircle_FS -o Circle_FS.h glsl/Circle_FS.glsl

glslangValidator --target-env vulkan1.0 -S geom -DKUAS_ANTIALIAS --vn ShaderCircleAA_GS -o CircleAA_GS.h glsl/Circle_GS.glsl
glslangValidator --target-env vulkan1.0 -S frag -DKUAS_ANTIALIAS --vn ShaderCircleAA_FS -o CircleAA_FS.h glsl/Circle_FS.glsl

REM Fill circle
glslangValidator --target-env vulkan1.0 -S vert -DKUAS_FILL_SHAPE --vn ShaderFillCircle_VS -o FillCircle_VS.h glsl/Circle_VS.glsl
glslangValidator --target-env vulkan1.0 -S geom -DKUAS_FILL_SHAPE --vn ShaderFillCircle_GS -o FillCircle_GS.h glsl/Circle_GS.glsl
glslangValidator --target-env vulkan1.0 -S frag -DKUAS_FILL_SHAPE --vn ShaderFillCircle_FS -o FillCircle_FS.h glsl/Circle_FS.glsl

glslangValidator --target-env vulkan1.0 -S geom -DKUAS_ANTIALIAS -DKUAS_FILL_SHAPE --vn ShaderFillCircleAA_GS -o FillCircleAA_GS.h glsl/Circle_GS.glsl
glslangValidator --target-env vulkan1.0 -S frag -DKUAS_ANTIALIAS -DKUAS_FILL_SHAPE --vn ShaderFillCircleAA_FS -o FillCircleAA_FS.h glsl/Circle_FS.glsl

REM Fill ellipse

glslangValidator --target-env vulkan1.0 -S vert -DKUAS_FILL_SHAPE --vn ShaderFillEllipse_VS -o FillEllipse_VS.h glsl/FillEllipse_VS.glsl
glslangValidator --target-env vulkan1.0 -S geom -DKUAS_FILL_SHAPE --vn ShaderFillEllipse_GS -o FillEllipse_GS.h glsl/FillEllipse_GS.glsl

glslangValidator --target-env vulkan1.0 -S geom -DKUAS_ANTIALIAS -DKUAS_FILL_SHAPE --vn ShaderFillEllipseAA_GS -o FillEllipseAA_GS.h glsl/FillEllipse_GS.glsl

REM Line segment
glslangValidator --target-env vulkan1.0 -S vert --vn ShaderLineSegment_VS -o LineSegment_VS.h glsl/LineSegment_VS.glsl
glslangValidator --target-env vulkan1.0 -S geom --vn ShaderLineSegment_GS -o LineSegment_GS.h glsl/LineSegment_GS.glsl
glslangValidator --target-env vulkan1.0 -S frag --vn ShaderLineSegment_FS -o LineSegment_FS.h glsl/LineSegment_FS.glsl

glslangValidator --target-env vulkan1.0 -S geom -DKUAS_ANTIALIAS --vn ShaderLineSegmentAA_GS -o LineSegmentAA_GS.h glsl/LineSegment_GS.glsl
glslangValidator --target-env vulkan1.0 -S frag -DKUAS_ANTIALIAS --vn ShaderLineSegmentAA_FS -o LineSegmentAA_FS.h glsl/LineSegment_FS.glsl

glslangValidator --target-env vulkan1.0 -S geom -DKUAS_ANTIALIAS -DKUAS_CAPPED_LINE --vn ShaderLineSegmentCappedAA_GS -o LineSegmentCappedAA_GS.h glsl/LineSegment_GS.glsl
glslangValidator --target-env vulkan1.0 -S frag -DKUAS_ANTIALIAS -DKUAS_ROUND_CAPPED_LINE --vn ShaderLineSegmentRoundedAA_FS -o LineSegmentRoundedAA_FS.h glsl/LineSegment_FS.glsl