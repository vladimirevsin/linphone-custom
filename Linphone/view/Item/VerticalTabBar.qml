import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2 as Control
import QtQuick.Effects

import Linphone

Control.TabBar {
	id: mainItem
	spacing: 15
	topPadding: 20

	property var model
	
	contentItem: ListView {
		model: mainItem.contentModel
		currentIndex: mainItem.currentIndex

		spacing: mainItem.spacing
		orientation: ListView.Vertical
		// boundsBehavior: Flickable.StopAtBounds
		flickableDirection: Flickable.AutoFlickIfNeeded
		// snapMode: ListView.SnapToItem

		// highlightMoveDuration: 0
		// highlightRangeMode: ListView.ApplyRange
		// preferredHighlightBegin: 40
		// preferredHighlightEnd: width - 40
	}

	background: Item {
		id: background
		anchors.fill: parent
		Rectangle {
			anchors.fill: parent
			color: DefaultStyle.verticalTabBarColor
			radius: 25
		}
		Rectangle {
			color: DefaultStyle.verticalTabBarColor
			anchors.left: parent.left
			anchors.top: parent.top
			width: parent.width/2
			height: parent.height/2
		}
		Rectangle {
			color: DefaultStyle.verticalTabBarColor
			x: parent.x + parent.width/2
			y: parent.y + parent.height/2
			width: parent.width/2
			height: parent.height/2
		}
	}

	Repeater {
		id: actionsRepeater
		model: mainItem.model
		Control.TabButton {
			id: tabButton
			width: mainItem.width

			contentItem: ColumnLayout {
				height: tabButton.height
				width: tabButton.width
				EffectImage {
					id: buttonIcon
					property int buttonSize: 20
					image.source: mainItem.currentIndex === index ? modelData.selectedIcon : modelData.icon
					Layout.preferredWidth: buttonSize
					Layout.preferredHeight: buttonSize
					Layout.alignment: Qt.AlignHCenter
					image.sourceSize.width: buttonSize
					image.fillMode: Image.PreserveAspectFit
					effect.brightness: 1.0
				}
				Text {
					id: buttonText
					text: modelData.label
					font {
						bold: mainItem.currentIndex === index
						pointSize: DefaultStyle.verticalTabButtonTextSize
					}
					color: DefaultStyle.verticalTabBarTextColor
					Layout.fillWidth: true
					Layout.preferredHeight: txtMeter.height
					Layout.alignment: Qt.AlignHCenter
					horizontalAlignment: Text.AlignHCenter
					leftPadding: 3
					rightPadding: 3
				}
			}
			TextMetrics {
				id: txtMeter
				text: modelData.label
				font: buttonText.font
				Component.onCompleted: {
					font.bold= true
					mainItem.implicitWidth = Math.max(mainItem.implicitWidth, advanceWidth + buttonIcon.buttonSize)
				}
			}

			background: Item {
			}
		}
	}
}
