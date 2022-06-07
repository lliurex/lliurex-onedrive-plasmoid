import QtQuick 2.6
import QtQuick.Layouts 1.12

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.components 2.0 as Components
import org.kde.kquickcontrolsaddons 2.0 as KQuickControlsAddons
import QtQuick.Controls 2.6 as QQC2

import org.kde.plasma.private.lliurexonedrive 1.0

Components.ListItem {
    id: spaceItem
    property string idSpace
    property string nameSpace
    property int statusSpace
    property bool isRunningSpace
    property bool localFolderWarning
    readonly property bool isTall: height > Math.round(PlasmaCore.Units.gridUnit * 2.5)

	enabled:true

	onContainsMouseChanged: {
        if (containsMouse) {
            listSpaceView.currentIndex = index
        } else {
            listSpaceView.currentIndex = -1
        }
        listSpaceView.forceActiveFocus()

    }

    Item{
    	id:label
    	height:30
        Components.Label{
            id:spaceName
            text:nameSpace
            width:265
            elide:Text.ElideMiddle
            anchors.verticalCenter: parent.verticalCenter
        }
        Image {
            id:spaceStatusIcon
            source:getStatusIcon(statusSpace,localFolderWarning)
            sourceSize.width:32
            sourceSize.height:32
            anchors.leftMargin:15
            anchors.left:spaceName.right
            anchors.verticalCenter:parent.verticalCenter
        }  
        Image {
            id:spaceRunningIcon
            source:{
                if (isRunningSpace){
                    "/usr/share/icons/breeze/status/16/media-playback-playing.svg"
                }else{
                    "/usr/share/icons/breeze/status/16/media-playback-stopped.svg"
                }

            }
            sourceSize.width:32
            sourceSize.height:32
            anchors.leftMargin:15
            anchors.left:spaceStatusIcon.right
            anchors.verticalCenter:parent.verticalCenter

        }     
        QQC2.ToolButton {
        	id:loadSpaceBtn
        	width:35
        	height:35
                      
            anchors{
                left: spaceRunningIcon.right
                top: spaceName.isTall? parent.top : undefined
	            verticalCenter: parent.verticalCenter
	            leftMargin:10
	            rightMargin:10
	        }
            icon.name:"configure"
            visible:spaceItem.ListView.isCurrentItem
            QQC2.ToolTip{
                id:btnToolTip
                text:i18n("Click to get details of this space")
            }
            /*onClicked:lliurexOneDriveWidget.goToSpace(spaceId)*/

       }
   } 

    function getStatusIcon(statusSpace,localFolderWarning){

        switch (statusSpace){
            case 0:
            case 1:
            case 2:
            case 3:
                if (!localFolderWarning){
                    return "/usr/share/icons/breeze/status/16/state-ok.svg"
                }else{
                    return "/usr/share/icons/breeze/status/16/state-warning.svg"
                }
                break;
            default:
                return "/usr/share/icons/breeze/status/16/state-warning.svg"
                break
        }

    }
    
    
}
