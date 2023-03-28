import QtQuick 2.6
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.6 as QQC2
import QtQml.Models 2.3
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.components 2.0 as Components

import org.kde.plasma.extras 2.0 as PlasmaExtras

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
	        QQC2.ToolButton {
	            height:35
	            width:355
	            Layout.rightMargin:optionsAccount.width/3 - 50
	            icon.name:"arrow-left.svg"
	            QQC2.ToolTip{
	                text:i18n("Back to main view")
	            }
	            onClicked:lliurexOneDriveWidget.manageNavigation(0)
	        } 

	        Components.Label{
	            id:headText
	            text:i18n("Space details")
	            font.italic:true
	            font.pointSize:11
	            Layout.fillWidth:true
	            Layout.alignment:Qt.AlignHCenter
	        }
	        QQC2.ToolButton {
	            id:configureBtn
	            height:35
	            width:35
	            Layout.rightMargin:5
	            icon.name:"configure.svg"
	            QQC2.ToolTip{
	                text:i18n("Manage space")
	            }                        onClicked:lliurexOneDriveWidget.launchOneDrive(true)
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
	        QQC2.ToolButton {
	            width:35
	            height:35
	            icon.name:"document-open-folder.svg"
	            onClicked:lliurexOneDriveWidget.openFolder()
	            QQC2.ToolTip{
	                text:i18n("Click to open folder")
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

	        QQC2.ToolButton {
	            width:35
	            height:35
	            icon.name:lliurexOneDriveWidget.syncStatus?"kt-stop.svg":"kt-start.svg"
	            enabled:!lliurexOneDriveWidget.lliurexOneDriveOpen && !lliurexOneDriveWidget.clickedSyncBtn
	            onClicked:lliurexOneDriveWidget.manageSync() 
	            QQC2.ToolTip{
	                text:lliurexOneDriveWidget.syncStatus?i18n("Click to stop synchronization"):i18n("Click to start synchronization")
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
	       
	        QQC2.ToolButton {
	            width:35
	            height:35
	            icon.name:"arrow-right.svg"
	            onClicked:{
	                lliurexOneDriveWidget.getLatestFiles()
	                
	                lliurexOneDriveWidget.manageNavigation(2)
	            }
	            QQC2.ToolTip{
	                text:i18n("Click to see the list")
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
	            text:i18n("Current log file size:")
	            Layout.rightMargin:5
	            
	        }

	        Components.Label{
	            id:logSizeValue
	            text:lliurexOneDriveWidget.logFileSize
	            Layout.fillWidth:true
	        }
	       
	        QQC2.ToolButton {
	            width:35
	            height:35
	            icon.name:"document-open-file.svg"
	            onClicked:{
	                lliurexOneDriveWidget.openLogFile()
	            }
	            QQC2.ToolTip{
	                text:i18n("Click to see log file")
	            }
	        } 
	    }
	}
      
} 
