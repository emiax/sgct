/*************************************************************************
Copyright (c) 2012-2015 Miroslav Andel
All rights reserved.

For conditions of distribution and use, see copyright notice in sgct.h
*************************************************************************/

#include <sgct/CylindricalProjection.h>
#include <sgct/SGCTSettings.h>
#include <sgct/Engine.h>
#include <sgct/MessageHandler.h>
#include <sgct/shaders/SGCTInternalSphericalProjectionShaders.h>
#include <sgct/shaders/SGCTInternalSphericalProjectionShaders_modern.h>
#include <sgct/helpers/SGCTStringFunctions.h>
#include <sstream>
#include <algorithm>

// For now, this class does only support vertical cylindrical mappings.
// TODO: Allow cylinder to be oriented horizontally.

sgct_core::CylindricalProjection::CylindricalProjection()
{
    if (sgct::Engine::instance()->isOGLPipelineFixed())
    {
        sgct::MessageHandler::instance()->print(sgct::MessageHandler::NOTIFY_ERROR,
            "CylindricalProjection: Legacy OpenGL not supported for this projection");
    }
}

sgct_core::CylindricalProjection::~CylindricalProjection()
{

}

/*!
Set angle of cylinder sector, in degrees
*/
void sgct_core::CylindricalProjection::setSectorAngle(float angle)
{
    mSectorAngle = glm::radians(angle);
}

/*!
Set fov orthogonal to cylindrical fov
*/
void sgct_core::CylindricalProjection::setHeight(float height)
{
    mHeight = height;
}

void sgct_core::CylindricalProjection::setCylindricalResolution(int res)
{
    mCylindricalResolution = res;
}

void sgct_core::CylindricalProjection::setHeightResolution(int res)
{
    mHeightResolution = res;
}

void sgct_core::CylindricalProjection::setNFrustums(int nFrustums)
{
    mNFrustums = nFrustums;
}

void sgct_core::CylindricalProjection::setOffset(const glm::vec3 & offset)
{
    mOffset = offset;
}

void sgct_core::CylindricalProjection::setBaseOffset(const glm::vec3 & baseOffset)
{
    mBaseOffset = baseOffset;
}

void sgct_core::CylindricalProjection::setRadius(float radius)
{
    mRadius = radius;
}

/*!
Update projection when aspect ratio changes for the viewport.
*/
void sgct_core::CylindricalProjection::update(float width, float height)
{

}

/*!
Render the non linear projection to currently bounded FBO
*/
void sgct_core::CylindricalProjection::render()
{
    if (sgct::Engine::instance()->isOGLPipelineFixed()) {
        return;
    }
 // ..
}

/*!
Render the enabled faces of the cubemap
*/
void sgct_core::CylindricalProjection::renderCubemap(std::size_t * subViewPortIndex)
{
    if (sgct::Engine::instance()->isOGLPipelineFixed()) {
        return;
    }
    // ..
}

void sgct_core::CylindricalProjection::initOneViewport()
{
    const int xRes = mCylindricalResolution;
    const int yRes = mHeightResolution;

    const float angle = 0;
}

void sgct_core::CylindricalProjection::initTwoViewports()
{
    const int xRes = mCylindricalResolution / 2;
    const int yRes = mHeightResolution;

    const float cylPartFov = mSectorAngle / 2.f;

    const float angleLeft = cylPartFov / 2.f;
    const float angleRight = -cylPartFov / 2.f;
}

void sgct_core::CylindricalProjection::initThreeViewports()
{
    const int xRes = mCylindricalResolution / 3;
    const int yRes = mHeightResolution;

    const float cylPartFov = mSectorAngle / 3.f;

    const float angleFront = 0;
    const float angleLeft = cylPartFov;
    const float angleRight = -cylPartFov;
}

void sgct_core::CylindricalProjection::initFourViewports()
{
    const int xRes = mCylindricalResolution / 4;
    const int yRes = mHeightResolution;

    const float cylPartFov = mSectorAngle / 4.f;

    const float angleBackLeft = cylPartFov / 2.f * 3.f * 2.f;
    const float angleFrontLeft = cylPartFov / 2.f * 3.f;
    const float angleFrontRight = -cylPartFov / 2.f * 3.f;
    const float angleBackRight = -cylPartFov / 2.f * 3.f * 2.f;
}


void sgct_core::CylindricalProjection::initTextures()
{

}

void sgct_core::CylindricalProjection::initVBO()
{
    
}

void sgct_core::CylindricalProjection::initViewports()
{

    float screenXMin = glm::cos(mSectorAngle / 2);
    float screenYMin = -glm::sin(mSectorAngle / 2);
    float screenYMax = glm::sin(mSectorAngle / 2);
    float screenZMin = -mHeight / 2;
    float screenZMax = mHeight / 2;

    glm::vec2 userToLeft = glm::vec2(screenXMin - mBaseOffset.x, screenYMax - mBaseOffset.y);
    glm::vec2 userToRight = glm::vec2(screenXMin - mBaseOffset.x, screenYMin - mBaseOffset.y);
    glm::vec2 userToRightBaseVertex = rightBaseVertex - mBaseOffset;


    if (mNFrustums == 0) {
        mNFrustums = glm::clamp(static_cast<int>(mSectorAngle / glm::half_pi<float>()), 1, 3) + 1;
        sgct::MessageHandler::instance()->print(sgct::MessageHandler::NOTIFY_INFO,
            "CylindricalProjection: Automatically setting number of frustums to %d", mNFrustums);
    } else if (mNFrustums < static_cast<int>(mSectorAngle / 180.f)) {
        sgct::MessageHandler::instance()->print(sgct::MessageHandler::NOTIFY_ERROR,
            "CylindricalProjection: Need at least %d frustums for a cylindrical fov of %f",
            static_cast<int>(mSectorAngle / 180) + 1,
            mSectorAngle
        );
    }
    
    switch (mNFrustums) {
    case 1: initOneViewport(); break;
    case 2: initTwoViewports(); break;
    case 3: initThreeViewports(); break;
    case 4: initFourViewports(); break;
    }

}


void sgct_core::CylindricalProjection::initShaders()
{

}
