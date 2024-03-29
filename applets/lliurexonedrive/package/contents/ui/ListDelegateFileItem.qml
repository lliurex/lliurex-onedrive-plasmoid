import QtQuick 2.6
import QtQuick.Layouts 1.12
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.components 2.0 as Components
import org.kde.plasma.components 3.0 as PC3
import org.kde.kquickcontrolsaddons 2.0 as KQuickControlsAddons

import org.kde.plasma.private.lliurexonedrive 1.0

Components.ListItem {
    id: fileItem
    property string fileName
    property string filePath
    property string fileDate
    property string fileTime
    readonly property bool isTall: height > Math.round(PlasmaCore.Units.gridUnit * 2.5)

	enabled:true

	onContainsMouseChanged: {
        if (containsMouse) {
            listView.currentIndex = index
        } else {
            listView.currentIndex = -1
        }
        listView.forceActiveFocus()

    }

    Item{
    	id:label
    	height:30
        Components.Label{
            id:fileText
            text:{
            	if (fileItem.ListView.isCurrentItem){
            		fileName+"\n"+i18n("Last modification: ")+fileDate+"-"+fileTime
            	}else{
            		fileName
            	}
            }
            font.bold:{
            	if (fileItem.ListView.isCurrentItem){
            		true
            	}else{
            		false
            	}

            }	
            width:listView.width-(searchBtn.width*1.6)
            anchors.verticalCenter: parent.verticalCenter
        }
        PC3.ToolButton {
        	id:searchBtn
        	width:35
        	height:35
                      
            anchors{
                left: fileText.right
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
