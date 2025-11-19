import QtQuick
import QtQuick.Layouts
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid 
import org.kde.plasma.components as Components
import org.kde.plasma.components as PC3
import org.kde.kquickcontrolsaddons as KQuickControlsAddons
import org.kde.kirigami as Kirigami

import org.kde.plasma.private.lliurexonedrive 1.0

Components.ItemDelegate {
    id: uploadedFileItem
    property string fileName
    property string filePath
    property string fileDate
    property string fileTime
    readonly property bool isTall: height > Math.round(Kirigami.Units.gridUnit * 2.5)

    enabled:true
    height:50
    width:parent?parent.width:310

    Item{
        id:label
        height:30
        width:310
        anchors.fill:parent
        MouseArea {
            id: mouseAreaOption
            anchors.fill: parent
            hoverEnabled:true
            propagateComposedEvents:true

            onEntered: {
                uploadedListView.currentIndex = index
            }
        }
        Components.Label{
            id:fileText
            text:{
                if (uploadedFileItem.ListView.isCurrentItem){
                    fileName+"\n"+i18n("Uploaded on: ")+fileDate+"-"+fileTime
                }else{
                    fileName
                }
            }
            font.bold:{
                if (uploadedFileItem.ListView.isCurrentItem){
                    true
                }else{
                    false
                }
            }	
            width:uploadedListView.width-50
            anchors.left:parent.left
            anchors.leftMargin:10
            anchors.verticalCenter: parent.verticalCenter
        }

    }     
}
