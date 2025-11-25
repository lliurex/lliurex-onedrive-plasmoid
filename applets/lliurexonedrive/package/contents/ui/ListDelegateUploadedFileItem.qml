import QtQuick 2.6
import QtQuick.Layouts 1.12
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.components 2.0 as Components
import org.kde.plasma.components 3.0 as PC3
import org.kde.kquickcontrolsaddons 2.0 as KQuickControlsAddons

import org.kde.plasma.private.lliurexonedrive 1.0

Components.ListItem {
    id: uploadedFileItem
    property string fileName
    property string filePath
    property string fileDate
    property string fileTime
    readonly property bool isTall: height > Math.round(PlasmaCore.Units.gridUnit * 2.5)

    enabled:true

    onContainsMouseChanged: {
        if (containsMouse) {
            uploadedListView.currentIndex = index
        } else {
            uploadedListView.currentIndex = -1
        }
        uploadedListView.forceActiveFocus()

    }

    Item{
        id:label
        height:30
        Column{
            id:labelRow
            width:uploadedListView.width-50
            anchors.verticalCenter:parent.verticalCenter
            anchors.leftMargin:10

            Components.Label{
                id:fileText
                text:fileName
                font.bold:uploadedFileItem.ListView.isCurrentItem?true:false 
                width:labelRow.width-10
                elide:Text.ElideMiddle  

            }

            Components.Label{
                id:fileData
                text: i18n("Uploaded on: ")+fileDate+"-"+fileTime
                font.bold:true
                visible:uploadedFileItem.ListView.isCurrentItem?true:false
                width:fileText.width 
            }
        }
        
   }     
    
}
