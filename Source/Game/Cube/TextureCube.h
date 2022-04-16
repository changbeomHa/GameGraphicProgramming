/*+===================================================================
  File:      CUBE1.H

  Summary:   Cube header file contains declarations of Cube class
             used for the lab samples of Game Graphics Programming
             course.

  Classes: Cube

  © 2022 Kyung Hee University
===================================================================+*/
#pragma once

#include "Common.h"

#include "Cube/BaseCube.h"

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Class:    Cube

  Summary:  A renderable 3d cube object

  Methods:  Update
              Overriden function that updates the cube every frame
            Cube
              Constructor.
            ~Cube
              Destructor.
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
class TextureCube : public BaseCube
{
public:
    TextureCube(const std::filesystem::path& textureFilePath);
    TextureCube(const TextureCube& other) = delete;
    TextureCube(TextureCube&& other) = delete;
    TextureCube& operator=(const TextureCube& other) = delete;
    TextureCube& operator=(TextureCube&& other) = delete;
    ~TextureCube() = default;

    virtual void Update(_In_ FLOAT deltaTime) override;
};