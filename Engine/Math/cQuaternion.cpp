// Include Files
//==============

#include "cQuaternion.h"

#include "sVector.h"

#include <cmath>
#include <Engine/Asserts/Asserts.h>

// Static Data Initialization
//===========================

namespace
{
    constexpr auto s_epsilon = 1.0e-9f;
}

// Interface
//==========

// RotateVector
//-------------

eae6320::Math::sVector eae6320::Math::cQuaternion::operator *(const sVector& i_rhs) const
{
    const auto num = m_x * 2.0f;
    const auto num2 = m_y * 2.0f;
    const auto num3 = m_z * 2.0f;
    const auto num4 = m_x * num;
    const auto num5 = m_y * num2;
    const auto num6 = m_z * num3;
    const auto num7 = m_x * num2;
    const auto num8 = m_x * num3;
    const auto num9 = m_y * num3;
    const auto num10 = m_w * num;
    const auto num11 = m_w * num2;
    const auto num12 = m_w * num3;
    sVector result;
    result.x = (1.0f - (num5 + num6)) * i_rhs.x + (num7 - num12) * i_rhs.y + (num8 + num11) * i_rhs.z;
    result.y = (num7 + num12) * i_rhs.x + (1.0f - (num4 + num6)) * i_rhs.y + (num9 - num10) * i_rhs.z;
    result.z = (num8 - num11) * i_rhs.x + (num9 + num10) * i_rhs.y + (1.0f - (num4 + num5)) * i_rhs.z;
    return result;
}

// Multiplication
//---------------

eae6320::Math::cQuaternion eae6320::Math::cQuaternion::operator *( const cQuaternion& i_rhs ) const
{
    return cQuaternion(
        ( m_w * i_rhs.m_w ) - ( ( m_x * i_rhs.m_x ) + ( m_y * i_rhs.m_y ) + ( m_z * i_rhs.m_z ) ),
        ( m_w * i_rhs.m_x ) + ( m_x * i_rhs.m_w ) + ( ( m_y * i_rhs.m_z ) - ( m_z * i_rhs.m_y ) ),
        ( m_w * i_rhs.m_y ) + ( m_y * i_rhs.m_w ) + ( ( m_z * i_rhs.m_x ) - ( m_x * i_rhs.m_z ) ),
        ( m_w * i_rhs.m_z ) + ( m_z * i_rhs.m_w ) + ( ( m_x * i_rhs.m_y ) - ( m_y * i_rhs.m_x ) ) );
}

// Inversion
//----------

void eae6320::Math::cQuaternion::Invert()
{
    m_x = -m_x;
    m_y = -m_y;
    m_z = -m_z;
}

eae6320::Math::cQuaternion eae6320::Math::cQuaternion::GetInverse() const
{
    return cQuaternion( m_w, -m_x, -m_y, -m_z );
}

// Normalization
//--------------

void eae6320::Math::cQuaternion::Normalize()
{
    const auto length = sqrt( ( m_w * m_w ) + ( m_x * m_x ) + ( m_y * m_y ) + ( m_z * m_z ) );
    EAE6320_ASSERTF( length > s_epsilon, "Can't divide by zero" );
    const auto length_reciprocal = 1.0f / length;
    m_w *= length_reciprocal;
    m_x *= length_reciprocal;
    m_y *= length_reciprocal;
    m_z *= length_reciprocal;
}

eae6320::Math::cQuaternion eae6320::Math::cQuaternion::GetNormalized() const
{
    const auto length = sqrt( ( m_w * m_w ) + ( m_x * m_x ) + ( m_y * m_y ) + ( m_z * m_z ) );
    EAE6320_ASSERTF( length > s_epsilon, "Can't divide by zero" );
    const auto length_reciprocal = 1.0f / length;
    return cQuaternion( m_w * length_reciprocal, m_x * length_reciprocal, m_y * length_reciprocal, m_z * length_reciprocal );
}

// Products
//---------

float eae6320::Math::Dot( const cQuaternion& i_lhs, const cQuaternion& i_rhs )
{
    return ( i_lhs.m_w * i_rhs.m_w ) + ( i_lhs.m_x * i_rhs.m_x ) + ( i_lhs.m_y * i_rhs.m_y ) + ( i_lhs.m_z * i_rhs.m_z );
}

// Access
//-------

eae6320::Math::sVector eae6320::Math::cQuaternion::CalculateForwardDirection() const
{
    const auto _2x = m_x + m_x;
    const auto _2y = m_y + m_y;
    const auto _2xx = m_x * _2x;
    const auto _2xz = _2x * m_z;
    const auto _2xw = _2x * m_w;
    const auto _2yy = _2y * m_y;
    const auto _2yz = _2y * m_z;
    const auto _2yw = _2y * m_w;

    return sVector( -_2xz - _2yw, -_2yz + _2xw, -1.0f + _2xx + _2yy );
}

// Initialization / Shut Down
//---------------------------

eae6320::Math::cQuaternion::cQuaternion( const float i_angleInRadians, const sVector& i_axisOfRotation_normalized )
{
    const auto theta_half = i_angleInRadians * 0.5f;
    m_w = cos( theta_half );
    const auto sin_theta_half = sin( theta_half );
    m_x = i_axisOfRotation_normalized.x * sin_theta_half;
    m_y = i_axisOfRotation_normalized.y * sin_theta_half;
    m_z = i_axisOfRotation_normalized.z * sin_theta_half;
}
