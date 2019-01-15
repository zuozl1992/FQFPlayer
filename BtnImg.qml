import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Button {
    id: trafficBtn
    property string imagePath
    property int buttonStatus: -1 //按钮状态 -1=掉线 0=在线 1~9=在线数量
    property bool showStatus: false
    style: ButtonStyle
    {
        background: Image {
            anchors.fill: parent
            anchors.margins: control.hovered ? (control.pressed ? 2 : 0) :  3
            source: imagePath
        }
    }
    Text {
        id: trafficBtnNum
        width: parent.width / 3
        height: parent.height / 3
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: buttonStatus == -1 ? "#d81e06" : "green"
        font.bold: true
        visible: showStatus
        text: ((buttonStatus == -1) ? qsTr("×") : (((buttonStatus == 0) ? qsTr("√") : String(buttonStatus))))
    }
}
