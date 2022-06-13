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
    	height:45
        Column{
            id:spaceRow
            anchors.verticalCenter: parent.verticalCenter

    		Components.Label{
    		    id:spaceName
    		    text:nameSpace
    		    width:270
    		    elide:Text.ElideMiddle
                font.bold:{
                    if (spaceItem.ListView.isCurrentItem){
                        true
                    }else{
                        false
                    }
                }	
    		}
    		Components.Label{
    			id:spaceStatusText
    			text:getStatusText(statusSpace)
    			width:270
    			elide:Text.ElideMiddle
    			visible:spaceItem.ListView.isCurrentItem
                font.italic:true
                font.bold:{
                    if (spaceItem.ListView.isCurrentItem){
                        true
                    }else{
                        false
                    }
                }
            }
        }
        Image {
            id:spaceStatusIcon
            source:getStatusIcon(statusSpace,localFolderWarning)
            sourceSize.width:32
            sourceSize.height:32
            anchors.leftMargin:15
            anchors.left:spaceRow.right
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
            onClicked:lliurexOneDriveWidget.goToSpace(idSpace)

       }
   } 

    function getStatusIcon(statusSpace,localFolderWarning){

        if (localFolderWarning){
            return "/usr/share/icons/breeze/status/16/state-warning.svg"
        }else{
            switch (statusSpace){
                case 0:
                case 3:
                    return "/usr/share/icons/breeze/status/16/state-ok.svg"
                    break;
                case 2:
                case 4:
                    return "/usr/share/icons/breeze/status/16/state-sync.svg"
                    break;
                default:
                    return "/usr/share/icons/breeze/status/16/state-warning.svg"
                    break
            }
               
     
        }

    }

    function getStatusText(statusSpace,localFolderWarning){

        var msg=""
        if (localFolderWarning){
            msg=i18n("Local folder has been deleted or emptied")

        }else{
            switch (statusSpace){
                case 0:
                    msg=i18n("All remote content synchronized")
                    break
                case 2:
                    msg=i18n("Remote content pending syncing")
                    break;
                case 4:
                    msg=i18n("Uploading pending changes")
                    break
                case -1:
                    msg=i18n("OneDrive API return an error")
                    break
                case -2:
                    msg=i18n("Unable to connect with OneDrive")
                    break
                case -4:
                    msg=i18n("Free space is 0")
                    break
                case -7:
                    msg=i18n("Authorization has expired")
                    break
                case -9:
                    msg=i18n("Microsoft OneDrive not available")
                    break
            }

        }
        return msg

    }
    
    
}
