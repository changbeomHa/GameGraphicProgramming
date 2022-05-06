#include "Cube/Cube.h"


/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   Cube::Cube

  Summary:  Constructor

  Args:     const XMFLOAT4& outputColor
              Default color of the cube
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
Cube::Cube(_In_ const XMFLOAT4& outputColor)
    : BaseCube(outputColor)
{
}
/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   Cube::Update

  Summary:  Updates the cube every frame

  Args:     FLOAT deltaTime
              Elapsed time

  Modifies: [m_world].
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
// Does nothing?
void Cube::Update(_In_ FLOAT deltaTime)
{
    /*
    static FLOAT s_totalTime = 0.0f;
    s_totalTime += deltaTime;

    m_world = XMMatrixTranslation(0.0f, XMScalarSin(s_totalTime), 0.0f) * XMMatrixRotationY(s_totalTime);
    */
}