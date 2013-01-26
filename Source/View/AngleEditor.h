/*
 Copyright (C) 2010-2012 Kristian Duske
 
 This file is part of TrenchBroom.
 
 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with TrenchBroom.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __TrenchBroom__AngleEditor__
#define __TrenchBroom__AngleEditor__

#include "View/SmartPropertyEditor.h"

#include <wx/wx.h>
#include <wx/glcanvas.h>

namespace TrenchBroom {
    namespace View {
        class AngleEditorCanvas : public wxGLCanvas {
        public:
            AngleEditorCanvas(wxWindow* parent, const int* attribs);

            void OnPaint(wxPaintEvent& event);
        };
        
        class AngleEditor : public SmartPropertyEditor {
        protected:
            virtual wxWindow* createVisual(wxWindow* parent);
            virtual void destroyVisual();
            virtual void updateVisual();
        public:
            AngleEditor(SmartPropertyEditorManager& manager);
        };
    }
}

#endif /* defined(__TrenchBroom__AngleEditor__) */
