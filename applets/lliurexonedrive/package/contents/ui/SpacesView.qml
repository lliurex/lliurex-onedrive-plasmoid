import QtQuick
import QtQuick.Layouts
import QtQml.Models
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid
import org.kde.plasma.components as Components
import org.kde.plasma.components as PC3
import org.kde.kirigami as Kirigami

import org.kde.plasma.private.lliurexonedrive 1.0

Rectangle{
	color:"transparent"
	GridLayout{
	    id:spacesLayout
	    rows: 2
	    flow: GridLayout.TopToBottom
	    rowSpacing:10
	    width:parent.width

	    Kirigami.InlineMessage {
	        id: warningMessage
	        visible:lliurexOneDriveWidget.hddFreeSpaceStatus!="HDD_OK"?true:false
	        text:lliurexOneDriveWidget.hddFreeSpaceStatus=="HDD_Error"?i18n("The available space in HDD is less than 5 GB. No more files will be synced"):i18n("The available space in HDD is less than 10 GB")
	        type:lliurexOneDriveWidget.hddFreeSpaceStatus=="HDD_Error"?Kirigami.MessageType.Error:Kirigami.MessageType.Warning;
	        implicitWidth:parent.width-10
	        Layout.leftMargin:5
	        Layout.rightMargin:5
	        Layout.topMargin: 10
	    }                  
	    PC3.ScrollView {
	        Layout.topMargin:10
	        Layout.bottomMargin:10
	        Layout.leftMargin:5
	        Layout.rightMargin:5
	        implicitWidth:parent.width-10
	        implicitHeight:warningMessage.visible?300:350
	        ListView{
	            id:listSpaceView
	            focus:true
	            model:lliurexOneDriveWidget.spacesModel
	            currentIndex: -1
	            boundsBehavior: Flickable.StopAtBounds
	            interactive: contentHeight > height
	            highlight: Rectangle { color: "#add8e6"; opacity:0.8;border.color:"#53a1c9" }
	            highlightMoveDuration: 0
	            highlightResizeDuration: 0
	            delegate: ListDelegateSpaceItem {
	                idSpace: model.id
	                nameSpace: model.name
	                statusSpace: model.status
	                isRunningSpace: model.isRunning
	                localFolderWarning: model.localFolderWarning
	                updateRequiredWarning:model.updateRequiredWarning
	            }
	            Kirigami.PlaceholderMessage { 
	                id: emptySpaceHint
	                anchors.centerIn: parent
	                width: parent.width - (Kirigami.Units.largeSpacing * 4)
	                visible: {
	                    if (listSpaceView.count === 0){
	                        return true
	                    }else{
	                        return false
	                    } 
	                }    
	                text: i18n("No spaces configured")
	            }
	           
	        }
	    }
	   
	}

} 
