// Header Files
//=============

#include "Half.h"

// Static Data Initialization
//===========================

namespace
{
	int const shift = 13;
	int const shiftSign = 16;

	int32_t const infN = 0x7F800000; // flt32 infinity
	int32_t const maxN = 0x477FE000; // max flt16 normal as a flt32
	int32_t const minN = 0x38800000; // min flt16 normal as a flt32
	int32_t const signN = 0x80000000; // flt32 sign bit

	int32_t const infC = infN >> shift;
	int32_t const nanN = (infC + 1) << shift; // minimum flt16 nan as a flt32
	int32_t const maxC = maxN >> shift;
	int32_t const minC = minN >> shift;
	int32_t const signC = signN >> shiftSign; // flt16 sign bit

	int32_t const mulN = 0x52000000; // (1 << 23) / minN
	int32_t const mulC = 0x33800000; // minN / (1 << (23 - shift))

	int32_t const subC = 0x003FF; // max flt32 subnormal down shifted
	int32_t const norC = 0x00400; // min flt32 normal down shifted

	int32_t const maxD = infC - maxC - 1;
	int32_t const minD = minC - subC - 1;

	union
	{
		float m_float;
		int32_t m_signedInteger;
		uint32_t m_unsignedInteger;
	}v, s;
}


uint16_t eae6320::Math::MakeHalfFromFloat(const float i_value)
{
	
	v.m_float = i_value;
	uint32_t sign = v.m_signedInteger & signN;
	v.m_signedInteger ^= sign;
	sign >>= shiftSign; // logical shift
	s.m_signedInteger = mulN;
	s.m_signedInteger = static_cast<int32_t>(s.m_float * v.m_float); // correct subnormals
	v.m_signedInteger ^= (s.m_signedInteger ^ v.m_signedInteger) & -(minN > v.m_signedInteger);
	v.m_signedInteger ^= (infN ^ v.m_signedInteger) & -((infN > v.m_signedInteger) & (v.m_signedInteger > maxN));
	v.m_signedInteger ^= (nanN ^ v.m_signedInteger) & -((nanN > v.m_signedInteger) & (v.m_signedInteger > infN));
	v.m_unsignedInteger >>= shift; // logical shift
	v.m_signedInteger ^= ((v.m_signedInteger - maxD) ^ v.m_signedInteger) & -(v.m_signedInteger > maxC);
	v.m_signedInteger ^= ((v.m_signedInteger - minD) ^ v.m_signedInteger) & -(v.m_signedInteger > subC);
	return v.m_unsignedInteger | sign;
}

float eae6320::Math::MakeFloatFromHalf(const uint16_t i_value)
{
	v.m_unsignedInteger = i_value;
	auto sign = v.m_signedInteger & signC;
	v.m_signedInteger ^= sign;
	sign <<= shiftSign;
	v.m_signedInteger ^= ((v.m_signedInteger + minD) ^ v.m_signedInteger) & -(v.m_signedInteger > subC);
	v.m_signedInteger ^= ((v.m_signedInteger + maxD) ^ v.m_signedInteger) & -(v.m_signedInteger > maxC);
	s.m_signedInteger = mulC;
	s.m_float *= v.m_signedInteger;
	const auto mask = -(norC > v.m_signedInteger);
	v.m_signedInteger <<= shift;
	v.m_signedInteger ^= (s.m_signedInteger ^ v.m_signedInteger) & mask;
	v.m_signedInteger |= sign;
	return v.m_float;
}