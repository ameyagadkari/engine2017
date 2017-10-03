/*
This class represents a half(half of float)
*/

#ifndef EAE6320_MATH_CHALF_H
#define EAE6320_MATH_CHALF_H

#include <cstdint>

// Class Declaration
//==================

namespace eae6320
{
	namespace Math
	{
		class cHalf
		{
			// Interface
			//==========

		public:
			static uint16_t MakeHalfFromFloat(const float i_value);
			static float MakeFloatFromHalf(const uint16_t i_value);

			// Data
			//=====

		private:
			union Bits
			{
				float m_float;
				int32_t m_signedInteger;
				uint32_t m_unsignedInteger;
			};
		};
	}
}

#endif //  EAE6320_MATH_CHALF_H