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
    id: fileItem
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
                listView.currentIndex = index
            }
        }
        Column{
            id:fileRow
            anchors.leftMargin:10
            anchors.verticalCenter: parent.verticalCenter
            anchors.left:parent.left
            width:label.width-(searchBtn.width*1.6+10)

            Components.Label{
                id:fileText
                text:fileName
                font.bold:fileItem.ListView.isCurrentItem?true:false
                width:fileRow.width-10
                elide:Text.ElideMiddle
            }
            Components.Label{
                id:fileData
                text:i18n("Last modification: ")+fileDate+"-"+fileTime
                visible:fileItem.ListView.isCurrentItem?true:false
                font.bold:true
                width:fileText.width
            }
              
        }

        PC3.ToolButton {
            id:searchBtn
            width:35
            height:35
            anchors{
                left: fileRow.right
                top: fileText.isTall? parent.top : undefined
                verticalCenter: parent.verticalCenter
                leftMargin:10
                rightMargin:10
            }
            icon.name:"document-open-recent"
            visible:fileItem.ListView.isCurrentItem
            PC3.ToolTip{
                id:searchTP
                text:i18n("Click to access the file location")
            }
            onClicked:{
                searchTP.hide()
                if (lliurexOneDriveWidget.checkIfFileExists(filePath)){
                    lliurexOneDriveWidget.goToFile(filePath)
                }else{
                    btnToolTip.text=i18n("File does not exist. Update the list")
                }   
            } 
        }
    }     
}
