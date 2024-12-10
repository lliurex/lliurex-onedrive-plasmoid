import QtQuick
import QtQuick.Layouts
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid
import org.kde.plasma.components as Components
import org.kde.plasma.components as PC3
import org.kde.plasma.extras as PlasmaExtras

import org.kde.plasma.private.lliurexonedrive 1.0

Rectangle{
	color:"transparent"

    GridLayout{
	    id: optionsAccount
	    rows: 8
	    flow: GridLayout.TopToBottom
	    rowSpacing:10
	    width:parent.width

	    RowLayout{
	        id:head
	        Layout.fillWidth:true
	        PC3.ToolButton {
	            height:35
	            width:355
	            Layout.rightMargin:optionsAccount.width/3 - 50
	            icon.name:"arrow-left.svg"
	            PC3.ToolTip{
	            	id:backTP
	                text:i18n("Back to main view")
	            }
	            onClicked:{
	            	backTP.hide()
	            	lliurexOneDriveWidget.manageNavigation(0)
	            }
	        } 

	        Components.Label{
	            id:headText
	            text:i18n("Space details")
	            font.italic:true
	            font.pointSize:11
	            Layout.fillWidth:true
	            Layout.alignment:Qt.AlignHCenter
	        }
	        PC3.ToolButton {
	            id:configureBtn
	            height:35
	            width:35
	            Layout.rightMargin:5
	            icon.name:"configure.svg"
	            PC3.ToolTip{
	            	id:configureTP
	                text:i18n("Manage space")
	            }
	            onClicked:{
	            	configureTP.hide()
	            	lliurexOneDriveWidget.launchOneDrive(true)
	            }
	        } 
	    } 
     	RowLayout{
	        id:spaceMailRow
	        Layout.fillWidth:true
	        Layout.leftMargin:5
	        Layout.rightMargin:5
	        Layout.bottomMargin:5

	        Components.Label{
	            id:spaceMail
	            text:i18n("Email associated:")
	            Layout.rightMargin:5
	        }
	        Components.Label{
	            id:spaceMailVa5lue
	            text:lliurexOneDriveWidget.spaceMail
	            Layout.fillWidth:true
	        }
	    }
	    RowLayout{
	        id:spaceTypeRow
	        Layout.fillWidth:true
	        Layout.leftMargin:5
	        Layout.rightMargin:5

	        Components.Label{
	            id:spaceType
	            text:i18n("Type:")
	            Layout.rightMargin:5

	        }
	        Components.Label{
	            id:spaceTypeValue
	            text:{
	                if (lliurexOneDriveWidget.spaceType=="onedrive"){
	                    "OneDrive"
	                }else{
	                    "SharePoint"
	                }
	            }
	            Layout.fillWidth:true
	        }
	    }
	    RowLayout {
	        id:folderRow
	        Layout.leftMargin:5
	        Layout.rightMargin:5
	        Layout.fillWidth:true

	        Components.Label{
	            id:oneDriveFolder
	            text:i18n("Local folder:")
	            Layout.rightMargin:5
	        }
	        Components.Label{
	            id:oneDriveFolderValue
	            text:lliurexOneDriveWidget.oneDriveFolder
	            Layout.preferredWidth:295
	            elide:Text.ElideMiddle
	        }
	        PC3.ToolButton {
	            width:35
	            height:35
	            icon.name:"document-open-folder.svg"
	            PC3.ToolTip{
	            	id:folderTP
	                text:i18n("Click to open folder")
	            }
	            onClicked:{
	            	folderTP.hide()
	            	lliurexOneDriveWidget.openFolder()
	            }
	        } 
	    }

	    RowLayout{
	        id:freeSpaceRow
	        Layout.fillWidth:true
	        Layout.leftMargin:5
	        Layout.rightMargin:5

	        Components.Label{
	            id:freeSpace
	            text:i18n("Free Space:")
	            Layout.rightMargin:5
	        }
	        Components.Label{
	            id:freeSpaceValue
	            text:{
	                if (lliurexOneDriveWidget.freeSpace!=""){
	                    lliurexOneDriveWidget.freeSpace
	                }else{
	                    i18n("Information not available")
	                }
	            }
	            Layout.fillWidth:true
	        }
	    }

	    RowLayout{
	        id:syncRow
	        Layout.fillWidth:true
	        Layout.leftMargin:5
	        Layout.rightMargin:5

	        Components.Label{
	            id:syncText
	            text:i18n("Synchronization:")
	            Layout.rightMargin:5
	        }

	        Components.Label{
	            id:syncStatus
	            text:{
	                if (lliurexOneDriveWidget.syncStatus){
	                    if(lliurexOneDriveWidget.clickedSyncBtn){
	                        i18n("Stopping...")
	                    }else{
	                        i18n("Running")
	                    }
	                }else{
	                    if(lliurexOneDriveWidget.clickedSyncBtn){
	                        i18n("Starting...")
	                    }else{
	                        i18n("Stopped")
	                    }

	                }
	            }

	            Layout.fillWidth:true
	        }

	        PC3.ToolButton {
	            width:35
	            height:35
	            icon.name:lliurexOneDriveWidget.syncStatus?"kt-stop.svg":"kt-start.svg"
	            enabled:!lliurexOneDriveWidget.lliurexOneDriveOpen && !lliurexOneDriveWidget.clickedSyncBtn
	            PC3.ToolTip{
	            	id:syncTP
	                text:lliurexOneDriveWidget.syncStatus?i18n("Click to stop synchronization"):i18n("Click to start synchronization")
	            }
	            onClicked:{
	            	syncTP.hide()
	            	lliurexOneDriveWidget.manageSync()
	            } 
	        } 
	    } 
	    RowLayout{
	        id:latestFiles
	        Layout.fillWidth:true
	        Layout.leftMargin:5
	        Layout.rightMargin:5

	        Components.Label{
	            id:latestText
	            text:i18n("Recently modified files")
	            Layout.fillWidth:true
	        }
	       
	        PC3.ToolButton {
	            width:35
	            height:35
	            icon.name:"arrow-right.svg"
	            PC3.ToolTip{
	            	id:filesTP
	                text:i18n("Click to see the list")
	            }
	           	onClicked:{
	           		filesTP.hide()
	                lliurexOneDriveWidget.getLatestFiles()
	                lliurexOneDriveWidget.manageNavigation(2)
	            }

	        } 
	    }

	    RowLayout{
	        id: logFile
	        Layout.fillWidth:true
	        Layout.leftMargin:5
	        Layout.rightMargin:5
	        visible:{
	        	if (lliurexOneDriveWidget.logFileSize!=""){
	        		true
	        	}else{
	        		false
	        	}
	        }

	        Components.Label{
	            id:logSizeT
	            text:i18n("Log file size:")
	            Layout.rightMargin:5
	            
	        }

	        Components.Label{
	            id:logSizeValue
	            text:lliurexOneDriveWidget.logFileSize
	            Layout.fillWidth:true
	        }
	       
	        PC3.ToolButton {
	            width:35
	            height:35
	            icon.name:"document-open-file.svg"
	            PC3.ToolTip{
	            	id:logTP
	                text:i18n("Click to see log file")
	            }
	            onClicked:{
	            	logTP.hide()
	                lliurexOneDriveWidget.openLogFile()
	            }

	        } 
	    }
	}
      
} 
