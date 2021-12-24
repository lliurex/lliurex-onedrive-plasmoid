import QtQuick 2.2
import QtQuick.Layouts 1.12

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.components 2.0 as Components
import org.kde.plasma.components 3.0 as Components3
import org.kde.plasma.private.lliurexonedrive 1.0

Components.ListItem {
    id: fileItem
    property string fileName
    property string filePath
    property string fileDate
    property string fileTime

    Rectangle{
        width:listView.width-10
        height:30
        color:"transparent"
        border.color:"transparent"
        opacity:1
        Components.Label{
            id:fileText
            text:"  "+fileName
            Components3.ToolTip.text:i18n("Last modification: ")+fileDate+"-"+fileTime+"\n"+i18n("Click to access the file location")
            Components3.ToolTip.visible:itemArea.containsMouse
            Components3.ToolTip.delay:500
            anchors.verticalCenter: parent.verticalCenter
        }
        MouseArea{
            id:itemArea
            anchors.fill:parent
            hoverEnabled: true
            onEntered:{
                parent.color="#add8e6"
                parent.border.color="#53a1c9"
                parent.opacity=0.8
            }
            onExited:{
                parent.color="transparent"
                parent.border.color="transparent"
                parent.opacity=1.0
            }
            onClicked:{
                lliurexOneDriveWidget.goToFile(filePath)
            }
        }

    }
}