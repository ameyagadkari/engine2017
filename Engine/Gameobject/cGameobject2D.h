/*
A gameobject 2d class is a representation for a 2D gameobject on screen
*/

#ifndef EAE6320_GAMEOBJECT_CGAMEOBJECT2D_H
#define EAE6320_GAMEOBJECT_CGAMEOBJECT2D_H

// Include Files
//==============

#include <Engine/Transform/sRectTransform.h>
#include <Engine/Graphics/cTexture.h>
#include <Engine/Graphics/cEffect.h>

// Forward Declarations
//=====================

namespace eae6320
{
    namespace Graphics
    {
        class cSprite;
    }
}

// Class Declaration
//==================

namespace eae6320
{
    namespace Gameobject
    {
        class cGameobject2D
        {
            // Interface
            //==========

        public:

            // Initialization / Clean Up
            //--------------------------

            static cResult Load(const char* const i_path, cGameobject2D*& o_gameobject2D, const int16_t i_x, const int16_t i_y, const uint16_t i_width, const uint16_t i_height, const Transform::eAnchor i_anchor, const char* const i_effectPath, const char* const i_textureMainPath, const char* const i_textureAlternatePath = nullptr);

            EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cGameobject2D);

            // Reference Counting
            //-------------------

            EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();

            cResult CleanUp();

            // Render
            //-------

            void BindAndDraw() const;

        private:

            // Initialization / Clean Up
            //--------------------------
            explicit cGameobject2D(const int16_t i_x, const int16_t i_y, const uint16_t i_width, const uint16_t i_height, const Transform::eAnchor i_anchor);
            ~cGameobject2D() { CleanUp(); }

            // Data
            //=====

            Transform::sRectTransform m_rectTransform;
            Graphics::cSprite* m_sprite;
            Graphics::cEffect::Handle m_effect;
            Graphics::cTexture::Handle m_textureMain;
            Graphics::cTexture::Handle m_textureAlternate;
            EAE6320_ASSETS_DECLAREREFERENCECOUNT();
        public:
            bool m_useAlternateTexture;
        };
    }
}

#endif // EAE6320_GAMEOBJECT_CGAMEOBJECT2D_H