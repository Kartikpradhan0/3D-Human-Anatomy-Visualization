# 3D Human Anatomy Visualization (C++ / OpenGL)

A beginner-friendly Computer Graphics project. It renders a stylized 3D human body,
a simplified skeleton, and selected internal organs. Use the mouse to select body
regions and read short educational descriptions.

## Requirements
- Windows 10/11
- MinGW-w64 g++
- FreeGLUT development package (headers and libraries)
- OpenGL and GLU (normally provided by Windows graphics SDK/runtime)

## Build
Open a terminal in this folder and run:

```bash
g++ main.cpp -o Anatomy3D.exe -lfreeglut -lopengl32 -lglu32
```

If the compiler cannot find `GL/freeglut.h`, install FreeGLUT and add its `include`
and `lib` directories to your MinGW paths. The FreeGLUT DLL must be available beside
the EXE or on the Windows PATH.

## Run
```bash
./Anatomy3D.exe
```

## Controls
- `1`: body surface only
- `2`: skeleton only
- `3`: organs only
- `B`: toggle body surface
- `S`: toggle skeleton overlay
- `O`: toggle organs
- Arrow keys: rotate
- `+` / `-`: zoom
- `R`: reset view
- Left mouse click: select a body region and show its description
- `Esc`: exit

## Notes
This is an educational, stylized model made from OpenGL primitives. It is not a
medically accurate anatomical model and should not be used for diagnosis or treatment.
