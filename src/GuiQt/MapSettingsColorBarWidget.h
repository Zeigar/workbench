#ifndef __MAP_SETTINGS_COLOR_BAR_WIDGET_H__
#define __MAP_SETTINGS_COLOR_BAR_WIDGET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/


#include <QWidget>



namespace caret {

    class AnnotationColorWidget;
    class AnnotationCoordinateWidget;
    class AnnotationFontWidget;
    class AnnotationWidthHeightWidget;
    class EnumComboBoxTemplate;
    
    class Overlay;
    
    class MapSettingsColorBarWidget : public QWidget {
        
        Q_OBJECT

    public:
        MapSettingsColorBarWidget(QWidget* parent = 0);
        
        virtual ~MapSettingsColorBarWidget();
        
        void updateContent(Overlay* overlay);
        

        // ADD_NEW_METHODS_HERE

    private slots:
        void applySelections();
        
        void annotationColorBarPositionModeEnumComboBoxItemActivated();
        
        void annotationCoordinateSpaceEnumComboBoxItemActivated();
        
    private:
        MapSettingsColorBarWidget(const MapSettingsColorBarWidget&);

        MapSettingsColorBarWidget& operator=(const MapSettingsColorBarWidget&);
        
        Overlay* m_overlay;
        
        EnumComboBoxTemplate* m_annotationColorBarPositionModeEnumComboBox;
        
        EnumComboBoxTemplate* m_annotationCoordinateSpaceEnumComboBox;
        
        AnnotationColorWidget* m_colorWidget;
        
        AnnotationCoordinateWidget* m_coordinateWidget;
        
        AnnotationFontWidget* m_fontWidget;
        
        AnnotationWidthHeightWidget* m_widthHeightWidget;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __MAP_SETTINGS_COLOR_BAR_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __MAP_SETTINGS_COLOR_BAR_WIDGET_DECLARE__

} // namespace
#endif  //__MAP_SETTINGS_COLOR_BAR_WIDGET_H__