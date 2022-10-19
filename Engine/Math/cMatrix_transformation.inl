#ifndef EAE6320_MATH_CMATRIX_TRANSFORMATION_INL
#define EAE6320_MATH_CMATRIX_TRANSFORMATION_INL

// Include Files
//==============

// #include "cMatrix_transformation.h"

// Interface
//==========

// Access
//-------

inline const eae6320::Math::sVector& eae6320::Math::cMatrixTransformation::GetRightDirection() const
{
    return *reinterpret_cast<const sVector*>( &m_00 );
}

inline const eae6320::Math::sVector& eae6320::Math::cMatrixTransformation::GetUpDirection() const
{
    return *reinterpret_cast<const sVector*>( &m_01 );
}

inline const eae6320::Math::sVector& eae6320::Math::cMatrixTransformation::GetBackDirection() const
{
    return *reinterpret_cast<const sVector*>( &m_02 );
}

inline const eae6320::Math::sVector& eae6320::Math::cMatrixTransformation::GetTranslation() const
{
    return *reinterpret_cast<const sVector*>( &m_03 );
}

// Camera
//-------

inline eae6320::Math::cMatrixTransformation eae6320::Math::cMatrixTransformation::CreateWorldToCameraTransform(
    const cQuaternion& i_cameraOrientation, const sVector& i_cameraPosition )
{
    return CreateWorldToCameraTransform( cMatrixTransformation( i_cameraOrientation, i_cameraPosition ) );
}

#endif    // EAE6320_MATH_CMATRIX_TRANSFORMATION_INL
