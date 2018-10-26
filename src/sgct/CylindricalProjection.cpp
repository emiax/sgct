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

#include <glm/gtx/vector_angle.hpp>

namespace {

void setupProjectionPlane(
    sgct_core::SGCTProjectionPlane* p,
    float leftFovAngle,
    float rightFovAngle,
    float top,
    float bottom)
{
	glm::vec4 lowerLeft = glm::vec4(
        -glm::sin(leftFovAngle),
        bottom,
        -glm::cos(leftFovAngle),
        1.f);

    glm::vec4 upperLeft = glm::vec4(
        -glm::sin(leftFovAngle),
        top,
        -glm::cos(leftFovAngle),
        1.f);

    glm::vec4 upperRight = glm::vec4(
        -glm::sin(rightFovAngle),
        top,
        -glm::cos(rightFovAngle),
        1.f);

    p->setCoordinate(sgct_core::SGCTProjectionPlane::LowerLeft, glm::vec3(lowerLeft));
    p->setCoordinate(sgct_core::SGCTProjectionPlane::UpperLeft, glm::vec3(upperLeft));
    p->setCoordinate(sgct_core::SGCTProjectionPlane::UpperRight, glm::vec3(upperRight));
};

}

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

glm::vec3 sgct_core::CylindricalProjection::totalOffset() const
{
    return mBaseOffset + mOffset;
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

void sgct_core::CylindricalProjection::initTextures()
{

}

void sgct_core::CylindricalProjection::initVBO()
{
    
}

void sgct_core::CylindricalProjection::initViewports()
{
    // In this limited context we refer to the plane where
    // x points into the screen and y points to the left.

    const float screenXMin = glm::cos(mSectorAngle / 2);
    const float screenYMin = -glm::sin(mSectorAngle / 2);
    const float screenYMax = glm::sin(mSectorAngle / 2);
	
    const glm::vec2 leftEdge = glm::vec2(screenXMin, screenYMax);
    const glm::vec2 rightEdge = glm::vec2(screenXMin, screenYMin);

    mFovAngleLeft = glm::orientedAngle(glm::vec2(1.f, 0.f), leftEdge);
    mFovAngleRight = glm::orientedAngle(glm::vec2(1.f, 0.f), rightEdge);


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

	// Now, we refer to OpenGL's 3D coordinate system where
	// x points to the left, y points up, and z points out of the screen.

    const float top = mHeight / 2.f - mOffset.y;
    const float bottom = -mHeight / 2.f - mOffset.y;
    const float frustumXFov = mSectorAngle / static_cast<float>(mNFrustums);

    for (int i = 0; i < mNFrustums; ++i)
    {
        const float leftAngle = (mFovAngleLeft - frustumXFov * i);
        const float rightAngle = (mFovAngleLeft - frustumXFov * (i + 1));
        sgct_core::SGCTProjectionPlane* p0 = mSubViewports[0].getProjectionPlane();
        setupProjectionPlane(p0, leftAngle, rightAngle, top, bottom);
    }
}


void sgct_core::CylindricalProjection::initShaders()
{
    //reload shader program if it exists
    if (mShader.isLinked())
        mShader.deleteProgram();

	// TODO: Load shaders
    std::string vertexShader = "";
    std::string fragmentShader = "";

    if (!mShader.addShaderSrc(vertexShader,
        GL_VERTEX_SHADER,
        sgct::ShaderProgram::SHADER_SRC_STRING))
    {
        sgct::MessageHandler::instance()->print(
            sgct::MessageHandler::NOTIFY_ERROR,
            "Failed to load fisheye vertex shader:\n%s\n",
            vertexShader.c_str());
    }

    if (!mShader.addShaderSrc(
        fragmentShader,
        GL_FRAGMENT_SHADER,
        sgct::ShaderProgram::SHADER_SRC_STRING))
    {
        sgct::MessageHandler::instance()->print(
            sgct::MessageHandler::NOTIFY_ERROR,
            "Failed to load fisheye fragment shader\n%s\n",
            fragmentShader.c_str());
    }

    mShader.setName("CylindricalShader");
    mShader.createAndLinkProgram();
    mShader.bind();
	
    // TODO: Set uniforms

    sgct::ShaderProgram::unbind();

}
